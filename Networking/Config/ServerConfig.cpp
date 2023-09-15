/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:47:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/15 17:07:24 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ServerConfig.hpp"

ServerConfig::ServerConfig(std::string serverBlock)
{
	domain = AF_INET;				// "Address Family - IPv4" |the socket will use the IPv4 addressing scheme
	service = SOCK_STREAM;			// Let the system decide the appropriate protocol based on the socket type (in this case, TCP for SOCK_STREAM)
	protocol = 0;					// The operating system chooses the appropriate protocol based on the previous settings. Given AF_INET and SOCK_STREAM, the OS will typically choose TCP.
	interface = INADDR_ANY;

	port = 80;
	host = "127.0.0.1";
	server_name = "localhost";
	client_max_body_size = 1 * 1024 * 1024;

	// TODO: check that error pages are .html files
	// Set other error pages, now they don't exist
	error_pages[0] = "./error_pages/default_error.html";

	// Default methods
	rootLocation.allow_methods.push_back("GET");
	rootLocation.allow_methods.push_back("POST");
	rootLocation.allow_methods.push_back("DELETE");
	rootLocation.autoindex = true;
	rootLocation.root = "./www";
	rootLocation.route = "/";
	rootLocation.index = "index.html";

	//parse, check, and attribute directives from the config file
	parseServerBlock(serverBlock);

}

ServerConfig::~ServerConfig()
{

}

// Receives the entire server{} block and parses all directives and location{} blocks
// then checks for remaining unkown directive
void ServerConfig::parseServerBlock(std::string serverBlock)
{
	std::string temp;
	if (containsDirective(serverBlock, "\nlisten "))
		port = extractPort(removeDirective(serverBlock,  "\nlisten "));
			
	if (containsDirective(serverBlock, "\nserver_name "))
		server_name = extractServer_name(removeDirective(serverBlock,  "\nserver_name "));
			
	if (containsDirective(serverBlock, "\nhost "))
		host = extractHost(removeDirective(serverBlock,  "\nhost "));
			
	if (containsDirective(serverBlock, "\nerror_page "))
		extractError_page(removeDirective(serverBlock,  "\nerror_page "));

	if (containsDirective(serverBlock, "\nclient_max_body_size "))
		client_max_body_size = extractClient_max_body_size(removeDirective(serverBlock,  "\nclient_max_body_size "));

	parseRootLocation(serverBlock);
	parseLocations(serverBlock);

	// Remove the first line containing '{'
	size_t startPos = serverBlock.find('{');
	if (startPos != std::string::npos)
		serverBlock.erase(startPos, 1);

	// Remove the last line containing '}'
	size_t endPos = serverBlock.rfind('}');
	if (endPos != std::string::npos)
		serverBlock.erase(endPos, 1);

	// remove the line containing "server"
	if (containsDirective(serverBlock, "server"))
		removeDirective(serverBlock,  "server");

	// check if there is something else remaining in the config file
	if (serverBlock.find_first_not_of(" \t\n\r\f\v{}") != std::string::npos)
	{
		std::cout << serverBlock << std::endl; // to test
		throw std::runtime_error("Unknown directive(s) --> " + serverBlock);
	}

}

// Look for a root location {} block, check that there's only one, and modify the rootLocation struct if needed
void ServerConfig::parseRootLocation(std::string &serverBlock)
{
	std::string::size_type startPos = 0, endPos = 0;
	int root_count = 0; // count the number of root location in the config file

	while ((startPos = serverBlock.find("\nlocation ", startPos)) != std::string::npos)
	{
		endPos = startPos + std::strlen("location ");;  // move past "location "

		// skip white spaces, until the "/"
		while (endPos < serverBlock.size() && isspace(serverBlock[endPos]))
   			++endPos;

		// check that the first char is a "/"
		if (endPos >= serverBlock.size() || serverBlock[endPos] != '/')
			throw std::runtime_error("Error: wrong location path");

		++endPos;

		// skip white spaces again, until the "{"
		while (endPos < serverBlock.size() && isspace(serverBlock[endPos]))
			++endPos;

		// If any other char than "{" is found --> not the root location
		if (endPos >= serverBlock.size() || serverBlock[endPos] != '{')
		{
			startPos = endPos;
			continue;
		}

		// throw an error if a root location was already found
		if (root_count > 0)
			throw std::runtime_error("Error: too many root locations!");
	
		root_count++;
		int bracketCount = 1;
		endPos++;
		
		// look for ending matching "}"
		while (bracketCount > 0 && endPos < serverBlock.size())
		{
			if (serverBlock[endPos] == '{')
				bracketCount++;
			else if (serverBlock[endPos] == '}')
				bracketCount--;
			endPos++;
		}
		
		// add the location {} block to the vector if bracketCount == 0
		// otherwise send an error
		if (bracketCount == 0)
			rootLocation = createLocation(serverBlock.substr(startPos, endPos - startPos));
		else
			throw std::runtime_error("Error: Unmatched brackets in config file!");

		// std::string locationBlock = serverBlock.substr(startPos, endPos - startPos + 1);

		// erase the location {} block
		serverBlock.erase(startPos, endPos - startPos);
	}
}

// find all location {} blocks which are not root location and create a location struct from them
void ServerConfig::parseLocations(std::string &serverBlock)
{
	std::string::size_type startPos = 0, endPos = 0;

	while ((startPos = serverBlock.find("\nlocation ", startPos)) != std::string::npos)
	{
		endPos = startPos + 9;  // move past "location "

		// skip all white space until "/"
		while ((serverBlock[endPos] == ' ' || serverBlock[endPos] == '\n' || serverBlock[endPos] == '\t') && endPos < serverBlock.size())
			endPos++;
		
		// Check if the first char is a "/"
		if (!serverBlock[endPos] || serverBlock[endPos] != '/')
			throw std::runtime_error("Error: problem with location in conf file!");

		// from the "/" to the "{", we have the URL route
		while ((serverBlock[endPos] != '{') && endPos < serverBlock.size())
			endPos++;

		// throw an error if no "{" was found
		if (endPos >= serverBlock.size())
			throw std::runtime_error("Error: problem with location in conf file!");

		int bracketCount = 1;
		endPos++;
		
		// look for ending matching "}"
		while (bracketCount > 0 && endPos < serverBlock.size())
		{
			if (serverBlock[endPos] == '{')
				bracketCount++;
			else if (serverBlock[endPos] == '}')
				bracketCount--;
			endPos++;
		}
		
		// add the location{} block to the vector if bracketCount == 0
		// otherwise send an error
		if (bracketCount == 0)
			locations.push_back(createLocation(serverBlock.substr(startPos, endPos - startPos)));
		else
			throw std::runtime_error("Error: Unmatched brackets in config file!");
		
		//std::string locationBlock = serverBlock.substr(startPos, endPos - startPos + 1);
		
		// erase the location {} block
		serverBlock.erase(startPos, endPos - startPos);

	}
}

// create a Location struct from the location {} block in the config file
// checks for index, root, path, autoindex and allow_methods
// if one of them is missing, the location attribute is set to default
struct Location ServerConfig::createLocation(std::string locStr)
{
	Location loc;

	std::string path = locStr.substr(9);  // skip "location "

	size_t startPos = locStr.find('/'); // the path (route) MUST start with a "/"
	size_t endPos = locStr.find('{', startPos);

	std::string extractedPath = locStr.substr(startPos, endPos - startPos);

	// std::cout << "str = ." << extractedPath << ".\n";
	while (isspace(extractedPath.back()))
		extractedPath.pop_back();
	// std::cout << "str = ." << extractedPath << ".\n";

	// Check if there is any white space in the route
	if (extractedPath.find_first_of(" \t\n\r\f\v") != std::string::npos)
		throw std::runtime_error("CONFIG ERROR: Wrong location path:" + extractedPath);
	
	loc.route = extractedPath;
	if (containsDirective(locStr, "\nredirect "))
	{
		Location redirect = extractRedirect(removeDirective(locStr,  "\nredirect "));

		loc.root = redirect.root;
		loc.autoindex = redirect.autoindex;
		loc.allow_methods = redirect.allow_methods;
		loc.index = redirect.index;
	}
	else
	{
		// check for the root directive, otherwise use the default root
		if (containsDirective(locStr, "\nroot "))
		{
			std::string tempRoot = extractRoot(removeDirective(locStr,  "\nroot "));

			// remove a "/" if both the tempRoot and the extractedPath already have one
			if (!tempRoot.empty() && tempRoot[tempRoot.size() - 1] == '/' && !extractedPath.empty() && extractedPath[0] == '/')
				tempRoot.pop_back();

			// add a "/" if neither the tempRoot nor the extractedPath have one
			else if (!tempRoot.empty() && tempRoot[tempRoot.size() - 1] != '/' && !extractedPath.empty() && extractedPath[0] != '/')
				tempRoot = tempRoot + '/';

			loc.root = tempRoot + extractedPath;
		}
		else
		{	
			// remove a "/" if both the rootLocation.root and the extractedPath already have one
			if (!rootLocation.root.empty() && rootLocation.root[rootLocation.root.size() - 1] == '/' && !extractedPath.empty() && extractedPath[0] == '/')
				rootLocation.root.pop_back();

			// add a "/" if neither the rootLocation.root nor the extractedPath have one
			else if (!rootLocation.root.empty() && rootLocation.root[rootLocation.root.size() - 1] != '/' && !extractedPath.empty() && extractedPath[0] != '/')
			rootLocation.root = rootLocation.root + '/';

			loc.root = rootLocation.root + extractedPath;
		}

		// check for the autoindex directive, otherwise use the default autoindex
		if (containsDirective(locStr, "\nautoindex "))
			loc.autoindex = extractAutoindex(removeDirective(locStr,  "\nautoindex "));
		else
			loc.autoindex = rootLocation.autoindex;

		// check for the allow_methods directive, otherwise use the default allow_methods
		if (containsDirective(locStr, "\nallow_methods "))
			loc.allow_methods = extractAllow_methods(removeDirective(locStr,  "\nallow_methods "));
		else
			loc.allow_methods = rootLocation.allow_methods;

		// check for the index directive, otherwise use the default index
		if (containsDirective(locStr, "\nindex "))
			loc.index = extractIndex(removeDirective(locStr,  "\nindex "), loc.root);
		else
			loc.index = rootLocation.index;	}
	
	// remove the line containing "location"
	if (containsDirective(locStr, "location"))
		removeDirective(locStr,  "location");

	// check if there is something else remaining in the config file
	if (locStr.find_first_not_of(" \t\n\r\f\v{}") != std::string::npos)
	{
		std::cout << locStr << std::endl; // to test
		throw std::runtime_error("Wrong location directive(s) --> " + locStr);
	}
	return (loc);
}

// extract redirection
struct Location ServerConfig::extractRedirect(std::string line)
{
	std::string redirect = line.substr(9);  // skip "redirect "

	redirect = trimWhiteSpaces(redirect);

	// Check if ends with semicolon
	if (!endsWithSemicolon(redirect))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	redirect.pop_back();
	redirect = trimWhiteSpaces(redirect);  // trim any more potential whitespace

	// if there is any white character within the remaining string, the path can't be valid
	if (redirect.find_first_of(" \t\n\r\f\v") != std::string::npos)
		throw std::runtime_error("CONFIG ERROR: Wrong redirection path: " + redirect);
	
	for (size_t i = 0; i < locations.size(); i++)
	{
		if (locations[i].route == redirect)
			return locations[i];
	}

	// std::cout << redirect << std::endl;

	throw std::runtime_error("CONFIG ERROR: Unset redirection path: " + redirect);
}

// extract & check autoindex from the line
bool ServerConfig::extractAutoindex(std::string line)
{
	std::string autoindex = line.substr(10);  // skip "autoindex "

	autoindex = trimWhiteSpaces(autoindex);

	// Check if ends with semicolon
	if (!endsWithSemicolon(autoindex))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	autoindex.pop_back();
	autoindex = trimWhiteSpaces(autoindex);  // trim any more potential whitespace

	// std::cout << autoindex << std::endl;

	// autoindex can either be on or off
	if (autoindex == "on")
		return true;
	else if (autoindex == "off")
		return false;
	else
		throw std::runtime_error("CONFIG ERROR: Wrong autoindex: " + autoindex);
}

// extract & check allow_methods from the line
std::vector<std::string> ServerConfig::extractAllow_methods(std::string line)
{
	std::string allow_methods = line.substr(14);  // skip "allow_methods "
	std::vector<std::string> result;

	allow_methods = trimWhiteSpaces(allow_methods);

	// Check if ends with semicolon
	if (!endsWithSemicolon(allow_methods))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	allow_methods.pop_back();
	allow_methods = trimWhiteSpaces(allow_methods);  // trim any more potential whitespace
	
	// Split the string into a vector of strings
	std::vector<std::string> methods = splitToVector(allow_methods);

	// check each substring for an accepted method
	for(size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == "GET")
			result.push_back("GET");
		else if (methods[i] == "POST")
			result.push_back("POST");
		else if (methods[i] == "DELETE")
			result.push_back("DELETE");
		else
			throw std::runtime_error("Unknown method: " + methods[i]);
	}

	// std::cout << allow_methods << std::endl;

	return (result);
}

// extract & check the root from the line
std::string ServerConfig::extractIndex(std::string line, std::string path)
{
	std::string index = line.substr(6);  // skip "index "

	index = trimWhiteSpaces(index);

	// Check if ends with semicolon
	if (!endsWithSemicolon(index))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	index.pop_back();
	index = trimWhiteSpaces(index);  // trim any more potential whitespace

	// if there is any white character within the remaining string, the path can't be valid
	if (index.find_first_of(" \t\n\r\f\v") != std::string::npos)
		throw std::runtime_error("CONFIG ERROR: Wrong index file name: " + index);
	
	// remove a "/" if both the path and the index already have one
	if (!path.empty() && path[path.size() - 1] == '/' && !index.empty() && index[0] == '/')
		path.pop_back();

	// add a "/" if neither the path nor the index have one
	else if (!path.empty() && path[path.size() - 1] != '/' && !index.empty() && index[0] != '/')
		path = path + '/';

	std::string checkAccess = path + index;
	
	// std::cout << "access = " << checkAccess << "PATH = " << path << " index = " << index << std::endl;
	if (access(checkAccess.c_str(), F_OK) != 0)  // Check if file exists --> 404 error
		throw std::runtime_error("Specified index file does not exist: " + checkAccess);

	// TODO: Do we keep only the index file's name, or its path as well with --> return (checkAccess);
	return (index);
}

// extract & check the root from the line
std::string ServerConfig::extractRoot(std::string line)
{
	std::string root = line.substr(5);  // skip "root "

	root = trimWhiteSpaces(root);

	// Check if ends with semicolon
	if (!endsWithSemicolon(root))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	root.pop_back();
	root = trimWhiteSpaces(root);  // trim any more potential whitespace

	// if there is any white character within the remaining string, the path can't be valid
	if (root.find_first_of(" \t\n\r\f\v") != std::string::npos)
		throw std::runtime_error("CONFIG ERROR: Wrong location path: " + root);
	
	if (access(root.c_str(), F_OK) != 0)  // Check if file exists
		throw std::runtime_error("Specified root does not exist: " + root);

	// std::cout << root << std::endl;

	return (root);
}

// extract & check the client_max_body_size from the line
int ServerConfig::extractClient_max_body_size(std::string line)
{
	std::string bodySize = line.substr(21);  // skip "client_max_body_size "

	bodySize = trimWhiteSpaces(bodySize);

	// Check if ends with semicolon
	if (!endsWithSemicolon(bodySize))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	bodySize.pop_back();
	bodySize = trimWhiteSpaces(bodySize);  // trim any more potential whitespace

	if (!isNumber(bodySize))
		throw std::runtime_error("Invalid client_max_body_size specification: not a number");
	
	int res = std::stoi(bodySize);
	
	// std::cout << res << std::endl;

	return (res);
}

// extract & check the error_pages from the line
void ServerConfig::extractError_page(std::string line)
{
	std::string errorStr = line.substr(11);  // skip "error_page "

	errorStr = trimWhiteSpaces(errorStr);

	// Check if ends with semicolon
	if (!endsWithSemicolon(errorStr))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	errorStr.pop_back();
	errorStr = trimWhiteSpaces(errorStr);  // trim any more potential whitespace

	// Split the string into a vector of strings 
	std::vector<std::string> tokens = splitToVector(errorStr);

	if (tokens.size() % 2 != 0)  // Ensure we have pairs
		throw std::runtime_error("Mismatched error codes and paths in error_page directive.");

	for (size_t i = 0; i < tokens.size(); i += 2) 
	{
		std::string errorCodeStr = tokens[i];
		std::string path = tokens[i + 1];

		if (!isNumber(errorCodeStr))
			throw std::runtime_error("Invalid error code in error_page directive: " + errorCodeStr);

		int errorCode = std::atoi(errorCodeStr.c_str());

		// TODO: Validate error codes based on the ones we support
		// TODO: create the error pages
		if (errorCode != 400 && errorCode != 403 && errorCode != 404 \
			&& errorCode != 405 && errorCode != 410 && errorCode != 413 && errorCode != 500 && errorCode != 501) 
			throw std::runtime_error("Unsupported error code in error_page directive: " + errorCodeStr);

		// Validate the path if required.
		if (access(path.c_str(), F_OK) != 0)  // Check if file exists
			throw std::runtime_error("Specified error page does not exist: " + path);

		// Store the valid error code and path into a map<>
		error_pages[errorCode] = path;
	}

	// std::cout << errorStr << std::endl;
}

// extract & check the port from the line
int ServerConfig::extractPort(std::string line)
{
	std::string portString = line.substr(7);  // skip "listen "

	portString = trimWhiteSpaces(portString);

	// Check if ends with semicolon
	if (!endsWithSemicolon(portString))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	portString.pop_back();
	portString = trimWhiteSpaces(portString);  // trim any more potential whitespace

	if (!isNumber(portString))
		throw std::runtime_error("Invalid port specification: not a number");
	
	int port = std::stoi(portString);
	
	if (!isValidPort(port))
		throw std::runtime_error("Invalid port number: out of range");

	// std::cout << port << std::endl;

	return (port);
}

// extract & check the host from the line
std::string ServerConfig::extractHost(std::string line)
{
	std::string host = line.substr(6);  // skip "host "

	host = trimWhiteSpaces(host);

	// Check if ends with semicolon
	if (!endsWithSemicolon(host))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	host.pop_back();
	host = trimWhiteSpaces(host);  // trim any more potential whitespace
	
	if (!isValidHost(host))
		throw std::runtime_error("Invalid host number.");

	// std::cout << host << std::endl;

	return (host);
}

// extract the server_name from the line
std::string ServerConfig::extractServer_name(std::string line)
{
	std::string server_name = line.substr(12);  // skip "server_name "

	server_name = trimWhiteSpaces(server_name);

	// Check if ends with semicolon
	if (!endsWithSemicolon(server_name))
		throw std::runtime_error("Directive does not end with a semicolon");

	// Remove the semicolon for parsing
	server_name.pop_back();
	server_name = trimWhiteSpaces(server_name);  // trim any more potential whitespace

	// NOTE: didn't added any check for the server name as it won't impact our program

	// std::cout << server_name << std::endl;

	return (server_name);
}


// a function that removes the line containing the directive once it has been found
// it returns the line in order to extract, check, and save its content
std::string ServerConfig::removeDirective(std::string &serverBlock, const std::string &directive)
{
	size_t pos = serverBlock.find(directive);

	if (pos != std::string::npos)
	{
		size_t startOfLine = pos++; // Start after the previous newline character
		size_t endOfLine = serverBlock.find('\n', pos + directive.length()); // Find the next newline character

		std::string extractedLine = serverBlock.substr(startOfLine, endOfLine - startOfLine);
		serverBlock.erase(startOfLine, endOfLine - startOfLine);
		return (extractedLine);
	}

	return (""); // Return an empty string if the directive is not found
}


// check if the string contains the directive
// returns 
// - true if it finds the string once
// - false if it doesn't
// - throws an error if it finds the directive more than once
bool ServerConfig::containsDirective(const std::string &serverBlock, const std::string &directive)
{
	int		occurrences = 0;
	size_t	pos = 0;

	while ((pos = serverBlock.find(directive, pos)) != std::string::npos)
	{
		++occurrences;
		pos += directive.length(); // Move past the directive that was just found.
	}
	if (occurrences == 1)
		return (true);
	else if (occurrences > 1)
	{
		std::string ret = directive;
		if (ret[0] == '\n')
			ret.erase(ret.begin());
		throw std::runtime_error("Directive declared more than once: " + ret);
	}
	else
	{
		// std::cout << "CONFIG: " << directive.substr(1) << " not found, set to default :)" << std::endl;
		return (false);
	}
}



// --------------- UTILS ---------------


// check if the string is a valid number
bool ServerConfig::isNumber(const std::string &str)
{
	if (str.empty())
		return (false);  // ensures the string is not empty
	
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!isdigit(*it))
			return (false);
	}
	return (true);
}

// check if the port is valid (must be a int between 0 and 65535)
bool ServerConfig::isValidPort(int port)
{
	if (port >= 0 && port <= 65535)
		return (true);
	return (false);
}

// check if the line ends with a semicolon
bool ServerConfig::endsWithSemicolon(const std::string &str)
{
	if (!str.empty() && str.back() == ';')
		return (true);
	return (false);
}

// remove all the tabs and spaces at the extremeties of the string
std::string ServerConfig::trimWhiteSpaces(const std::string &str)
{
	std::size_t first = str.find_first_not_of(" \t");

	if (std::string::npos == first) // if the string is only made of white spaces, return the string
		return (str);

	std::size_t last = str.find_last_not_of(" \t");

	return (str.substr(first, (last - first + 1)));
}

// check if the host is valid (e.g. 127.0.0.1)
bool ServerConfig::isValidHost(const std::string& host)
{
	std::istringstream	ss(host);
	std::string			segment;
	int					count = 0; // count number of dots

	// split into segments separated by a dot and check if it's a valid int between 0 and 255
	while (getline(ss, segment, '.'))
	{
		if (++count > 4)
			return (false);

		int num = atoi(segment.c_str());
		if (num < 0 || num > 255)
			return (false);

		// make sure no extra characters exist in the segment
		for (std::string::iterator it = segment.begin(); it != segment.end(); ++it)
		{
			if (!isdigit(*it)) 
				return (false);
		}
	}

	if (count == 4)
		return (true);
	return (false);
}

// Split a string into a vector for each word
std::vector<std::string> ServerConfig::splitToVector(const std::string &str)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, ' '))  // Split based on spaces first
	{
		if (!token.empty())
		{
			std::stringstream ss2(token);
			std::string subToken;
			
			while (std::getline(ss2, subToken, '\t'))  // Then split based on tabs
			{
				if (!subToken.empty()) 
					tokens.push_back(subToken);
			}
		}
	}

	return tokens;
}




// --------------- GETTERS & SETTERS ---------------

int ServerConfig::get_port()
{
	return (this->port);
}

int ServerConfig::get_domain()
{
	return (this->domain);
}

std::string ServerConfig::get_host()
{
	return (this->host);
}

std::string ServerConfig::get_server_name()
{
	return (this->server_name);
}

std::string ServerConfig::get_index()
{
	return (this->index);
}

std::map<int, std::string> ServerConfig::get_error_pages()
{
	return (this->error_pages);
}

int ServerConfig::get_client_max_body_size()
{
	return (this->client_max_body_size);
}

Location ServerConfig::get_rootLocation()
{
	return (this->rootLocation);
}

std::vector<Location> ServerConfig::get_locations()
{
	return (this->locations);
}

int ServerConfig::get_service()
{
	return (this->service);
}

int ServerConfig::get_protocol()
{
	return (this->protocol);
}

u_long ServerConfig::get_interface()
{
	return (this->interface);
}

std::string ServerConfig::get_endpoint()
{
	std::string res = this->host + ':' + std::to_string(this->port);

	return (res);
}
