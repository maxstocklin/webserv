/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responder.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:30:27 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/20 22:03:08 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Responder.hpp"

Responder::Responder(Handler &handler, std::map<int, std::string> errorMap, int new_socket) : errorMap(errorMap), new_socket(new_socket)
{
	statusMessage[200] = "OK";							// The server cannot or will not process the request due to something that is perceived to be a client error (e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
	statusMessage[400] = "Bad Request";					// The server cannot or will not process the request due to something that is perceived to be a client error (e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
	statusMessage[403] = "Forbidden";  					// The server understood the request, but it refuses to fulfill it. Authorization will not help and the request should not be repeated.
	statusMessage[404] = "Not Found";  					// The server can't find the requested resource. Links which lead to a 404 page are often called broken or dead links.
	statusMessage[405] = "Method Not Allowed"; 			// The request method is known by the server but is not supported by the target resource.
	statusMessage[408] = "Request Timeout";   			// This is a standard HTTP status code that indicates that the server timed out waiting for the request.
	statusMessage[410] = "Gone";   						// This response is sent when the requested content has been permanently deleted from the server, with no forwarding address.
	statusMessage[413] = "Payload Too Large";  			// The request is larger than the server is willing or able to process.
	statusMessage[500] = "Internal Server Error";  		// The server encountered an unexpected condition that prevented it from fulfilling the request.
	statusMessage[501] = "Not Implemented";  			// The server does not support the functionality required to fulfill the request.

	respond(handler);
}

Responder::~Responder()
{

}


void Responder::respond(Handler &handler)
{
	std::string	response_headers;

	response_headers = createResponseHeader(handler);

	if (handler.get_handler_response().statusCode != 200)
	{
		handler.set_response_htmlBody(get_error_content(handler.get_handler_response().statusCode));
		handler.set_response_keepAlive(false); // TODO --> Do we close all error connections?
		handler.set_response_htmlContentType("text/html");
	}

	// 1. Build the headers including the content length
	size_t temp = (handler.get_handler_response().htmlBody.size()) ;
	std::cout<< "***** LENG *****\n" << temp << std::endl;
	std::string headers = response_headers + "Content-Length: " + std::to_string(handler.get_handler_response().htmlBody.size()) + "\r\n\r\n";

	// 2. Combine headers and content
	std::string fullResponse = headers + handler.get_handler_response().htmlBody;

	if (temp < 1000)
	{
		std::cout << std::endl << std::endl << "###################### HTTP RESPONSE ######################" << std::endl  << std::endl;
		std::cout << fullResponse << std::endl;

	}
	// 3. Send the response
	ssize_t bytesSent = write(this->new_socket, fullResponse.data(), fullResponse.size());
	if (bytesSent == -1)
		throw std::runtime_error("ERROR: Error with write.");

	// std::cout << std::endl << std::endl << "###################### HTTP RESPONSE ######################" << std::endl  << std::endl;
	// std::cout << fullResponse << std::endl;

	// TODO: test for chunked data:
	// sendChunkedResponse(this->new_socket, fullResponse)
}



void Responder::sendChunkedResponse(int socket, const std::string& content)
{
    std::string headers = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n";
    write(socket, headers.data(), headers.size());

    size_t position = 0;
    const size_t chunkSize = 4096; // 4KB, but you can adjust as needed

    while (position < content.size()) {
        size_t currentChunkSize = std::min(chunkSize, content.size() - position);
        
        // Send chunk size in hex
        std::string chunkHeader = std::to_string(currentChunkSize) + "\r\n";
        write(socket, chunkHeader.data(), chunkHeader.size());
        
        // Send chunk data
        write(socket, content.data() + position, currentChunkSize);
        write(socket, "\r\n", 2);

        position += currentChunkSize;
    }

    // Send final zero chunk to indicate end
    write(socket, "0\r\n\r\n", 5);
}

#include <iomanip>

std::string Responder::createResponseHeader(Handler &handler)
{
	std::string response_headers;

	// Add the HTTP version and status code
	response_headers += "HTTP/1.1 ";

	if (statusMessage.find(handler.get_handler_response().statusCode) != statusMessage.end())
		response_headers += std::to_string(handler.get_handler_response().statusCode) + " " + statusMessage[handler.get_handler_response().statusCode];
	else
		response_headers += std::to_string(404) + " " + statusMessage[404]; // Default to 404 for unkown codes

	response_headers += "\r\n";

	// Add the Content-Type
	response_headers += "Content-Type: " + handler.get_handler_response().htmlContentType + "\r\n";

	// Add the keepAlive flag
	if (handler.get_handler_response().keepAlive)
		response_headers += "Connection: keep-alive\r\n";
	else
		response_headers += "Connection: close\r\n";

	return (response_headers);
}


std::string Responder::get_error_content(int statusCode)
{
	std::string errorPageContent;

	std::cout << "err statusCode = " << statusCode << std::endl;
	if (errorMap.find(statusCode) != errorMap.end())
		errorPageContent = loadFile(errorMap[statusCode]);
	else
		errorPageContent = loadFile(errorMap[0]);

	if (errorPageContent.empty())
		throw std::runtime_error("ERROR: Error with error file NTHING.");

	return (errorPageContent);
}

std::string Responder::loadFile(std::string errorFile)
{
	int op = 0;
	int readbytes = 0;
	char buffer[4096];

	memset(buffer, 0, sizeof(buffer));
	std::cout << "err file = " << errorFile << std::endl;

	// TODO: What to do when even the error files won't open or read?
	if ((op = open(errorFile.c_str(), O_RDONLY)) < 0)
		throw std::runtime_error("ERROR: Error with error file OPEN.");

	if ((readbytes = read(op, buffer, sizeof(buffer))) < 0)
		throw std::runtime_error("ERROR: Error with error file READ.");
	
	buffer[readbytes] = 0;
	return (buffer);	
}

void	Responder::setServer(ListeningSocket &server)
{
	_server = server;
}

void	Responder::setRequest(Handler &req)
{
	request = req;
}

bool Responder::checkError()
{
	if(request.get_handler_response().statusCode != 0)
	{
		_code = request.get_handler_response().statusCode;
		return (false);
	}
	return (true);
}




void	Responder::buildResponse()
{
	if (reqError() || buildBody()) // check if an error code was already set or if --> receives a 1
		buildErrorBody();

	else if (_auto_index)
	{
		std::cout << "AUTO index " << std::endl;
		if (buildHtmlIndex(_target_file, _body, _body_length))
		{
			_code = 500;
			buildErrorBody();
		}
		else
			_code = 200;
		_response_body.insert(_response_body.begin(), _body.begin(), _body.end());
	}
	setStatusLine();
	setHeaders();
	if (request.getMethod() != HEAD && (request.getMethod() == GET || _code != 200))
		_response_content.append(_response_body);
}



int    Responder::buildBody()
{
	_server.get
	if (request.getBody().length() > _server.get_client_max_body_size()) // do getBody()
	{
		_code = 413;
		return (1);
	}
	if (handleTarget())
		return (1);
	if (_cgi || _auto_index)
		return (0);
	if (_code)
		return (0);
	if (request.getMethod() == GET)
	{
		if (readFile())
			return (1);
	}
	else if (request.getMethod() == POST)
	{
		if (fileExists(_target_file))
		{
			_code = 204;
			return (0);
		}
		std::ofstream file(_target_file.c_str(), std::ios::binary);
		if (file.fail())
		{
			_code = 404;
			return (1);
		}

		if (request.getMultiformFlag())
		{
			std::string body = request.getBody();
			body = removeBoundary(body, request.getBoundary());
			file.write(body.c_str(), body.length());
		}
		else
		{
			file.write(request.getBody().c_str(), request.getBody().length());
		}
	}
	else if (request.getMethod() == DELETE)
	{
		if (!fileExists(_target_file))
		{
			_code = 404;
			return (1);
		}
		if (remove( _target_file.c_str() ) != 0 )
		{
			_code = 500;
			return (1);
		}
	}
	_code = 200;
	return (0);
}




 // todo check server pointer?
void Responder::makeFullLocalPath(ListeningSocket *master_socket, std::string &location_key, std::string &path, Location &target_location)
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
			location_key = locationVector[i].route;
		}
		std::cout << locationVector[i].route << " " << locationVector[i].root << std::endl;
	}
	if (found_loc == false)
	{
		target_location = master_socket->get_rootLocation();
		std::string first = master_socket->get_rootLocation().root;

		// remove a "/" if both the first and the path have one
		if (!first.empty() && first[first.size() - 1] == '/' && !path.empty() && path[0] == '/')
			first.pop_back();

		// add a "/" if neither the first nor the path have one
		else if (!first.empty() && first[first.size() - 1] != '/' && !path.empty() && path[0] != '/')
			first = first + '/';

		fullLocalPath = first + path;
		base_index = master_socket->get_rootLocation().index;
		location_key = master_socket->get_rootLocation().route;
	}
};


/* checks is the file exists and accessable */
int	Responder::checkFile(std::string const path, int mode)
{
	return (access(path.c_str(), mode));
}

/* check a file for CGI (the extension is supported, the file exists and is executable) and run the CGI */
int        Responder::handleCgi(Location &target_location)
{
	std::string path;
	std::string exten;
	size_t      pos;

	path = this->request.get_path();
	if (path[0] && path[0] == '/')
		path.erase(0, 1);

	// remove a "/" if both the path and the target_location.index have one
	if (!path.empty() && path[path.size() - 1] == '/' && !target_location.index.empty() && target_location.index[0] == '/')
		path.pop_back();

	// add a "/" if neither the path nor the target_location.index have one
	else if (!path.empty() && path[path.size() - 1] != '/' && !target_location.index.empty() && target_location.index[0] != '/')
			path = path + '/';
	path.append(target_location.index);

	// check if the index is not a file
	pos = path.find(".");
	if (pos == std::string::npos)
	{
		_code = 501;
		return (1);
	}
	exten = path.substr(pos);
	if (exten != ".php")
	{
		_code = 501;
		return (1);
	}

	if (request.isFile(path) != true)
	{
		_code = 404;
		return (1);
	}

	if (ConfigFile::checkFile(path, 1) == -1 || ConfigFile::checkFile(path, 3) == -1)
	{
		_code = 403;
		return (1);
	}

	_cgi_obj.clear();
	_cgi_obj.setCgiPath(path);
	_cgi = 1;
	if (pipe(_cgi_fd) < 0)
	{
		_code = 500;
		return (1);
	}
	_cgi_obj.initEnv(request, target_location); // + URI
	_cgi_obj.execute(this->_code);
	return (0);
}


// todo1: create base_index, fullLocalPath and location_key in hpp
// add string _location
// todo1: check how to get the path
int    Responder::handleTarget()
{
	std::string	location_key;
	Location target_location;
	// getLocationMatch(request.get_path(), _server.get_locations(), location_key);
	makeFullLocalPath(_server, location_key, request.get_path(), target_location);

	// TODO: REMOVE THIS if 
	if (location_key.length() > 0)
	{
		// check method
		int meth = 0;
		for (size_t j = 0; j < locationVector[i].allow_methods.size(); j++)
		{
			if (target_location.allow_methods[j] == method)
				meth = 1;
		}
		if (meth == 0)
		{
			_code = 405;
			return (1);
		}

		// check max body size
		if (request.getBody().length() > target_location.get_client_max_body_size())
		{
			_code = 413;
			return (1);
		}

		// INFO: We already managed redirections in config parsing
		// if (checkReturn(target_location, _code, _location))
		// 	return (1);

		if (target_location.root.find("cgi-bin") != std::string::npos)
		{
			return (handleCgi(location_key));
		}

		if (!target_location.getAlias().empty())
		{
			replaceAlias(target_location, request, _target_file);
		}
		else
			appendRoot(target_location, request, _target_file);

		if (!target_location.getCgiExtension().empty())
		{

			if (_target_file.rfind(target_location.getCgiExtension()[0]) != std::string::npos)
			{
				return (handleCgiTemp(location_key));
			}

		}
		if (isDirectory(_target_file))
		{
			if (_target_file[_target_file.length() - 1] != '/')
			{
				_code = 301;
				_location = request.getPath() + "/";
				return (1);
			}
			if (!target_location.getIndexLocation().empty())
				_target_file += target_location.getIndexLocation();
			else
				_target_file += _server.getIndex();
			if (!fileExists(_target_file))
			{
				if (target_location.getAutoindex())
				{
					_target_file.erase(_target_file.find_last_of('/') + 1);
					_auto_index = true;
					return (0);
				}
				else
				{
					_code = 403;
					return (1);
				}
			}
			if (isDirectory(_target_file))
			{
				_code = 301;
				if (!target_location.getIndexLocation().empty())
					_location = combinePaths(request.getPath(), target_location.getIndexLocation(), "");
				else
					_location = combinePaths(request.getPath(), _server.getIndex(), "");
				if (_location[_location.length() - 1] != '/')
					_location.insert(_location.end(), '/');

				return (1);
			}
		}
	}
	else
	{
		_target_file = combinePaths(_server.getRoot(), request.getPath(), "");
		if (isDirectory(_target_file))
		{
			if (_target_file[_target_file.length() - 1] != '/')
			{
				_code = 301;
				_location = request.getPath() + "/";
				return (1);
			}
			_target_file += _server.getIndex();
			if (!fileExists(_target_file))
			{
				_code = 403;
				return (1);
			}
			if (isDirectory(_target_file))
			{
				_code = 301;
				_location = combinePaths(request.getPath(), _server.getIndex(), "");
				if(_location[_location.length() - 1] != '/')
					_location.insert(_location.end(), '/');
				return (1);
			}
		}
	}
	return (0);
}
