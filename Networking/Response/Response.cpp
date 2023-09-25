/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:30:02 by max               #+#    #+#             */
/*   Updated: 2023/09/25 21:18:14 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Response.hpp"

// TODO UTILS

static std::string	to_string(size_t n)
{
	std::stringstream tmp;

	tmp << n;

	return tmp.str();
}


int		pathIsFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return 0;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

// Static Assets

void	Response::makeFullLocalPath(MasterSocket *master_socket, const std::string &path, const std::string &method, Location &target_location)
{
	std::cout << "\n\n__FULL PATH__\n\n";
	std::cout << "path = " << path << std::endl;
	std::vector<Location> locationVector = master_socket->get_locations();
	
	std::string basePath;

	// check if the path is the root path
	int secondBackslashPos = path.substr(1).find("/");
	if (secondBackslashPos != -1 || (secondBackslashPos == -1 && path.size() > 1))
	{
		basePath = path.substr(1, secondBackslashPos);
	} else
	{
		basePath = "/";
	}

	bool found_loc = false;
	for (unsigned long i = 0;  i < locationVector.size(); i++)
	{
		if (!locationVector[i].route.compare("/" + basePath))
		{
			found_loc = true;
			target_location = locationVector[i];
			int meth = 0;
			for (size_t j = 0; j < locationVector[i].allow_methods.size(); j++)
			{
				if (locationVector[i].allow_methods[j] == method)
					meth = 1;
			}
			if (meth == 0)
				_code = 405;
			
			std::string first = locationVector[i].root;
			std::string sec = path.substr(basePath.length() + 1);
			
			// remove a "/" if both the first and the path have one
			if (!first.empty() && first[first.size() - 1] == '/' && !sec.empty() && sec[0] == '/')
				first.pop_back();

			// add a "/" if neither the first nor the sec have one
			else if (!first.empty() && first[first.size() - 1] != '/' && !sec.empty() && sec[0] != '/')
				first = first + '/';

			fullLocalPath = first + sec;
			base_index = locationVector[i].index;
		}
	}
	if (found_loc == false)
	{
		target_location = master_socket->get_rootLocation();
		std::string first = master_socket->get_rootLocation().root;

		int meth = 0;
		for (size_t j = 0; j < master_socket->get_rootLocation().allow_methods.size(); j++)
		{
			if (master_socket->get_rootLocation().allow_methods[j] == method)
				meth = 1;
		}
		if (meth == 0)
			_code = 405;

		// remove a "/" if both the first and the path have one
		if (!first.empty() && first[first.size() - 1] == '/' && !path.empty() && path[0] == '/')
			first.pop_back();

		// add a "/" if neither the first nor the path have one
		else if (!first.empty() && first[first.size() - 1] != '/' && !path.empty() && path[0] != '/')
			first = first + '/';

		fullLocalPath = first + path;
		base_index = master_socket->get_rootLocation().index;
	}
};

// RESPONSE UTILS


// Function to extract the file extension
std::string Response::getFileExtension(const std::string& filename)
{
	size_t dotPos = filename.find_last_of(".");
	if (dotPos == std::string::npos) {
		return "";  // No extension found
	}
	return filename.substr(dotPos);
}


std::map<std::string, std::string> Response::getMimeTypes()
 {
	std::map<std::string, std::string> mimeTypes;

	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".php"] = "text/html";
	mimeTypes[".pdf"] = "application/pdf";




	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".mp4"] = "video/mp4";
	// mimeTypes[".pdf"] = "application/pdf";
	// mimeTypes[".pdf"] = "application/pdf";
	// mimeTypes[".pdf"] = "application/pdf";
	// mimeTypes[".pdf"] = "application/pdf";


	return mimeTypes;
}

std::string Response::getMimeType(const std::string& filePath)
{
	static const std::map<std::string, std::string> mimeTypes = getMimeTypes();

	std::string ext = getFileExtension(filePath);

	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // Convert to lowercase

	std::map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
	if (it != mimeTypes.end()) {
		return it->second;
	}

	return "what the fuck";  // default MIME type for unknown extensions
}


bool Response::isDirectory(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (S_ISDIR(s.st_mode))
			return true;
	}
	return false;
}

bool Response::isFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (S_ISREG(s.st_mode))
			return true;
	}
	return false;
}






// this function finds what to do with the request
// it sets the _responseType attribute to:
	// 0 = directory listing
	// 1 = normal html/jpeg body or CGI
	// 2 = Error
void Response::getPathResponse(MasterSocket &master_socket, Location target_location)
{
	std::string pathToCheck;

	// TODO: create an access function
	if (access(fullLocalPath.c_str(), F_OK) != 0)
	{
		if (errno == ENOENT)
			_code = 404;  // Not Found
		else if (errno == EACCES)
			_code = 403;  // Forbidden
		else
			_code = 500;  // Internal Server Error
		return;
	}
	
	// if the path is a directory, try to append the default file (index.html) otherwise check for autoindex
	if (!isFile(fullLocalPath))
	{
		if (fullLocalPath[fullLocalPath.size() - 1] != '/')
			fullLocalPath += "/";
		std::string append_index = fullLocalPath + base_index;

		// if the default file doesnt exist and the auto index is off --> error
		if (access(append_index.c_str(), F_OK) != 0 && !target_location.autoindex)
		{
			
			if (errno == ENOENT)
				_code = 404;  // Not Found
			else if (errno == EACCES)
				_code = 403;  // Forbidden
			else
				_code = 500;  // Internal Server Error
			_responseType = 2;
			_mimeType = "text/html";
			_usePath = fullLocalPath;	// use the path + default/index file

			return;
		}

		// if the default file doesnt exist and the auto index is on --> show directory listing
		else if (access(append_index.c_str(), F_OK) != 0 && master_socket.get_rootLocation().autoindex)
		{
			_responseType = 0;	// directory listing
			_mimeType = "text/html";
			_usePath = fullLocalPath;
			// FetchHtmlBody::dispatchResponse(*this, fullLocalPath, "text/html");
		}

		// if the default file exists --> show index file
		else
		{
			_responseType = 1;	// normal html/jpeg body or CGI
			_mimeType = getMimeType(append_index);	// check the mimeType??
			_usePath = append_index;	// use the path + default/index file

			// handler_response.htmlContentType = "text/html";
			// std::string mimeType2 = getMimeType(append_index);
			// FetchHtmlBody::dispatchResponse(*this, append_index, "text/html");
		}

	}
	// if the path is a file, check if it's a .php for CGI, and html/jpeg/png for normal body, or an error
	else
	{
		std::string mimeType = getMimeType(fullLocalPath);
		if (mimeType == "what the fuck")
		{
			_code = 501;
			_responseType = 2;
			_mimeType = "text/html";
			_usePath = fullLocalPath;	// use the path + default/index file
		}
		else
		{
			_responseType = 1; // CGI or html/jpeg body
			_mimeType = mimeType;
			_usePath = fullLocalPath;	// use the path + default/index file

			// FetchHtmlBody::dispatchResponse(*this, fullLocalPath, mimeType);
		}
	}
}


// Member functions

void			Response::call(Request &request, MasterSocket &requestConf)
{
	Location	target_location;

	// here, target_location is the right location block
	// private attribute "fullLocalPath" is the correct physical root in my computer
	// private attribute "base_index" is the correct index file
	makeFullLocalPath(&requestConf, request.getPath(), request.getMethod(), target_location);
	requestConf.set_path(this->fullLocalPath);

	_errorMap = requestConf.get_error_pages();
	_isAutoIndex = target_location.autoindex;
	_code = request.getRet();
	_hostPort.host = requestConf.get_host_int();	// todo: right now, get_host_int() always returns 0
	_hostPort.port = requestConf.get_port();
	_path = requestConf.get_rootLocation().root;


	if (static_cast<std::string::size_type>(requestConf.get_client_max_body_size()) < request.getBody().size() && _code != 405)
		_code = 413; // Payload Too Large

	if (_code == 405 || _code == 413)
	{
		ResponseHeader	head;
		// TODO: requestConf.getLang() is replaced by "", we won't handle this, so remove all occurrence
		_response = head.notAllowed(target_location.allow_methods, request.getPath(), _code, "") + "\r\n";
		return ;
	}
	

	if (request.getMethod() == "GET" || request.getMethod() == "POST")
	{
		getPathResponse(requestConf, target_location);
		requestConf.set_path(_usePath);	// in case index was appended
	}
	

	// TODO111: we don't check the path until this point, some parsing & checking are missing from:
	// requestConf = conf.getConfigForRequest(this->_listen,  request.getPath(), request.getHeaders().at("Host"), request.getMethod(), request);

	if (request.getMethod() == "GET")
		getHandler(request, requestConf);
	else if (request.getMethod() == "POST") 
		postHandler(request, requestConf);
	else if (request.getMethod() == "DELETE") 
		deleteHandler(request, requestConf);
	else
		throw std::runtime_error("Error with response: Method - " + request.getMethod());
}































void	Response::htmlResponder()
{
	int op = 0;
	ssize_t readbytes = 1;

	// todo: BUFFER_SIZE define
	char buffer[10000]; // consider using a dynamic approach or a larger buffer

	op = open(_usePath.c_str(), O_RDONLY);
	if (op == -1)
	{
		if (errno == ENOENT)
			_code = (404);  // Not Found
		else if (errno == EACCES)
			_code = (403);  // Forbidden
		else
			_code = (500);  // Internal Server Error
		_responseType = 2;	// useful?
		_mimeType = "text/html"; // useful?
		return;
	}
	while (readbytes > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		readbytes = read(op, buffer, sizeof(buffer));
		_response.append(buffer, readbytes);
	}
	if (readbytes == -1)
	{
		_code = (500);  // Internal Server Error
		_responseType = 2;	// useful?
		_mimeType = "text/html"; // useful?
		return;
	}

	_code = (200);
	// if (handler.get_connection() == "keep-alive")
	// 	handler.set_response_keepAlive(true);
	// else if (handler.get_connection() == "close")
	// 	handler.set_response_keepAlive(false);
};






#include <string>
#include <sys/stat.h>

std::string	Response::findExecutablePath(const std::string& command, char** env)
{
    std::string pathString;

    // Extract PATH from env
    while (*env)
	{
        std::string tmp(*env);
        if (tmp.substr(0, 5) == "PATH=")
		{
            pathString = tmp.substr(5); // Exclude "PATH=" part
            break;
        }
        env++;
    }

    if (pathString.empty())
	{
        // Couldn't find PATH variable
        return "";
    }

    // Search for the executable in each directory listed in PATH
    int start = 0;
    int end = 0;
    while (end != -1)
	{
        end = pathString.find(":", start);
        std::string testPath = pathString.substr(start, (end == -1 ? pathString.length() : end) - start) + "/" + command;
        start = end + 1;

        struct stat sb;
        if ((stat(testPath.c_str(), &sb) == 0 && (sb.st_mode & S_IXUSR))) {
            // Found the executable
            return testPath;
        }
    }

    // Couldn't find the executable
    return "";
}


// Methods
void			Response::getHandler(Request &request, MasterSocket &requestConf)
{
	ResponseHeader	head;

	std::string phpPath = findExecutablePath("php", _env);
	if (phpPath.empty())
	{
		std::cerr << RED << "Couldn't locate PHP." << RESET << std::endl;	// throw an error?
		_code = 500;
	}

	if ((_usePath.size() > 4 && !_usePath.substr(_usePath.size() - 4).compare(".php")))
	{
		CgiHandler	cgi(request, requestConf);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_response = cgi.executeCgi(phpPath);
		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else if (isFile(_usePath))
		htmlResponder();
	else if  (_code == 200)	// lsResponder(handler, _usePath);
	{
		std::stringstream	buffer;
		buffer << AutoIndexGenerator::getPage(_usePath.c_str(), to_string(_hostPort.host), _hostPort.port, request.getPath());
		_response = buffer.str();
		_mimeType = "text/html";
	}
	else
	{
		_mimeType = "text/html"; // todo: check for every time _type is set and replacew with mimetype
		_response = this->readHtml(_errorMap[_code]);
	}
	if (_code == 500)
	{
		_mimeType = "text/html";
		_response = this->readHtml(_errorMap[_code]);
	}

	_response = head.getHeader(_response.size(), _path, _code, _mimeType, request.getPath(), "") + "\r\n" + _response;
}


void			Response::postHandler(Request & request, MasterSocket & requestConf)
{
	ResponseHeader	head;


	if (requestConf.getCgiPass() != "")
	{
		CgiHandler	cgi(request, requestConf);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_response = cgi.executeCgi(requestConf.getCgiPass());

		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else
	{
		_code = 204;
		_response = "";
	}
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);
	_response = head.getHeader(_response.size(), _path, _code, _type, request.getPath(), "") + "\r\n" + _response;
}


void			Response::deleteHandler(Request &request, MasterSocket &requestConf)
{
	ResponseHeader	head;
	(void)request;
	(void)requestConf;

	_response = "";
	if (pathIsFile(_path))
	{
		if (remove(_path.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		_response = this->readHtml(_errorMap[_code]);
	_response = head.getHeader(_response.size(), _path, _code, _type, request.getPath(), "") + "\r\n" + _response;


}



// Utils


int				Response::writeContent(std::string content)
{
	std::ofstream	file;

	if (pathIsFile(_path))
	{
		file.open(_path.c_str());
		file << content;
		file.close();
		return (204);
	}
	else
	{
		file.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);

		file << content;
		file.close();
		return (201);
	}
}

std::string		Response::readHtml(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

		buffer << file.rdbuf();
		file.close();
		_type = "text/html";

		return (buffer.str());
	}
	else
		return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");
}

int				Response::fileExists(std::string path) //deprecated, replaced by ::pathIsFile()
{
	struct stat		stats;

	if (stat(path.c_str(), &stats) == 0)
		return (1);
	return (0);
}

// Getter functions

std::string		Response::getResponse(void)
{
	return (_response);
}

// Overloaders

Response & Response::operator=(const Response & src)
{
	_response = src._response;
	_path = src._path;
	_code = src._code;
	return (*this);
}

// Constructors and destructors

Response::Response(void)
{
}
Response::Response(char **env) : _env(env)
{
}
Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void)
{
}
