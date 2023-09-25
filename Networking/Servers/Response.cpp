/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:30:02 by max               #+#    #+#             */
/*   Updated: 2023/09/25 02:13:16 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Response.hpp"

// TODO UTILS


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

std::map<std::string, void (Response::*)(Request &, MasterSocket &)>	Response::initMethods()
{
	std::map<std::string, void (Response::*)(Request &, MasterSocket &)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;
	
	return map;
}

std::map<std::string, void (Response::*)(Request &, MasterSocket &)> Response::_method = Response::initMethods();



void	Response::makeFullLocalPath(MasterSocket *master_socket, const std::string &path, const std::string &method, Location &target_location)
{
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
		std::cout << locationVector[i].route << " " << locationVector[i].root << std::endl;
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

// Member functions

void			Response::call(Request &request, MasterSocket &requestConf)
{



	// std::cout << "request.getPath() == " << request.getPath() << std::endl;
	Location target_location;
	makeFullLocalPath(&requestConf, request.getPath(), request.getMethod(), target_location);
	requestConf.set_path(this->fullLocalPath);
	// here, target_location is the right location block
	// private attribute "fullLocalPath" is the correct physical root in my computer
	// private attribute "base_index" is the correct index file

	requestConf.get_host();
	_errorMap = requestConf.get_error_pages();
	_isAutoIndex = requestConf.get_rootLocation().autoindex;	// diff: our auto index is based on the Location block
	_code = request.getRet();
	_hostPort.host = requestConf.get_host_int();
	_hostPort.port = requestConf.get_port();
	_path = requestConf.get_rootLocation().root;

	// if (requestConf.getAllowedMethods().find(request.getMethod()) == requestConf.getAllowedMethods().end())
	// 	_code = 405;

	if (requestConf.get_client_max_body_size() < request.getBody().size() && _code != 405)
		_code = 413;


	// TODO: SKIPED FOR NOW
	if (_code == 405 || _code == 413)
	{
		ResponseHeader	head;
		// TODO: requestConf.getLang() is replaced by "", we won't handle this, so remove all occurrence
		_response = head.notAllowed(target_location.allow_methods, request.getPath(), _code, "") + "\r\n";
		return ;
	}


	(this->*Response::_method[request.getMethod()])(request, requestConf);
}

// Methods
void			Response::getMethod(Request & request, MasterSocket &requestConf)
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
			else if (str.find("Content-type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else if  (_code == 200)
		_code = readContent();
	else
		_response = this->readHtml(_errorMap[_code]);
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);

	_response = head.getHeader(_response.size(), _path, _code, _type, request.getPath(), "") + "\r\n" + _response;
}


void			Response::postMethod(Request & request, MasterSocket & requestConf)
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


void			Response::deleteMethod(Request & request, MasterSocket & requestConf)
{
	ResponseHeader	head;
	(void)request;

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



// TODO: UTILS
std::string	to_string(size_t n)
{
	std::stringstream tmp;

	tmp << n;

	return tmp.str();
}

// Utils

int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_response = "";

	if (pathIsFile(_path))
	{
		file.open(_path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			_response = this->readHtml(_errorMap[403]);
			return (403);
		}

		buffer << file.rdbuf();
		_response = buffer.str();

		file.close();
	}
	else if (_isAutoIndex) {
		buffer << AutoIndexGenerator::getPage(_path.c_str(),\
			to_string(_hostPort.host), _hostPort.port);
		_response = buffer.str();
		_type = "text/html";
	}
	else
	{
		_response = this->readHtml(_errorMap[404]);
		return (404);
	}

	return (200);
}

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

Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void)
{
}
