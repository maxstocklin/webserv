#include "../../Includes/Handler.hpp"


Handler::Handler()
{
	handler_response.statusCode = 0;
	
}

Handler::~Handler()
{

}

void Handler::setBuffer(std::string completeData) 
{
	_completeData = completeData;
	_chunk_flag = false;
	_body_flag = false;
	contentLength = 0;

	// strcpy(_buffer, buffer);
}

std::string getStringPiece(const std::string &line, int part) {
    // std::cout << "line = " << line;
    std::size_t start = 0;
    std::size_t end = 0;
    int count = 0;

    while (end != std::string::npos)
    {
        end = line.find(" ", start);

        if (count == part) // Check if we've reached the desired part
        {
            std::string piece;
            if (end == std::string::npos)
                piece = line.substr(start);
            else
                piece = line.substr(start, end - start);

            // Trim any trailing \r or \n characters
            while (!piece.empty() && (piece.back() == '\r' || piece.back() == '\n'))
                piece.pop_back();

            return piece;
        }

        start = end + 1;
        count++;
    }

    return ""; // If the part is not found, return an empty string.
}


std::map<std::string, std::string> Handler::getParameters(std::string params){
	int start = 0;
	int end = params.find("&");
	std::map<std::string, std::string> paramMap;
	while (end != -1) {
		int equalPosition = params.substr(start, end - start).find("=");
		paramMap[params.substr(start, end - start).substr(0, equalPosition)] = params.substr(start, end - start).substr(equalPosition + 1);
		start = end + 1;
		end = params.find("&", start);
	}
	int equalPosition = params.substr(start).find_last_of("=");
	if (equalPosition > 0)
		paramMap[params.substr(start).substr(0, equalPosition)] = params.substr(start).substr(equalPosition + 1);

	return (paramMap);
};

void Handler::getExecutablePath(std::string command)
{
	std::string pathString;
	for (unsigned long i = 0; i < cgiEnvVector.size(); i++)
	{
		if (!cgiEnvVector[i].substr(0,5).compare("PATH="))
		{
			pathString = cgiEnvVector[i];
		}
	}
	int start = 5;
	int end = 0;
	while (end != -1) {
		std::string testPath;
		end = pathString.find(":", start);
		exec_info.found = false;
		testPath = pathString.substr(start, (end == -1 ? pathString.length() : end)  - start) +"/" +command;
		start = end + 1;
		struct stat sb;
		if ((stat(testPath.c_str(), &sb) == 0 && (sb.st_mode & S_IXUSR)))
		{
			exec_info.found = true;
			exec_info.path = testPath.c_str();
			return;  // Return early once executable is found

		}
	}
	// TODO: error handling if path is not found
}

void Handler::makeFullLocalPath(ListeningSocket *master_socket, std::string &location_key, std::string &path, Location &target_location)
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
				handler_response.statusCode = 405;
			
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

		int meth = 0;
		for (size_t j = 0; j < master_socket->get_rootLocation().allow_methods.size(); j++)
		{
			if (master_socket->get_rootLocation().allow_methods[j] == method)
				meth = 1;
		}
		if (meth == 0)
			handler_response.statusCode = 405;

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


bool Handler::isDirectory(const std::string& path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0) {
		if (S_ISDIR(s.st_mode)) {
			return true;
		}
	}
	return false;
}

bool Handler::isFile(const std::string& path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0) {
		if (S_ISREG(s.st_mode)) {
			return true;
		}
	}
	return false;
}

void Handler::getPathResponse(ListeningSocket *master_socket)
{

	std::string pathToCheck;

	// TODO: create a checker function
	if (contentLength > master_socket->get_client_max_body_size())
		handler_response.statusCode = 413; // Payload Too Large

	if (handler_response.statusCode != 0)
	{
		handler_response.htmlContentType = "text/html";
		return;
	}

	if (access(fullLocalPath.c_str(), F_OK) != 0)
	{
		if (errno == ENOENT)
			handler_response.statusCode = 404;  // Not Found
		else if (errno == EACCES)
			handler_response.statusCode = 403;  // Forbidden
		else
			handler_response.statusCode = 500;  // Internal Server Error
		handler_response.htmlContentType = "text/html";
		return;
	}

	if (isDirectory(fullLocalPath))
		std::cout << fullLocalPath << " is a directory." << std::endl;
	else if (isFile(fullLocalPath))
		std::cout << fullLocalPath << " is a file." << std::endl;
	else
		std::cout << "Unknown type or does not exist." << std::endl;

	if (!isFile(fullLocalPath))
	{
		if (fullLocalPath[fullLocalPath.size() - 1] != '/')
			fullLocalPath += "/";
		std::string append_index = fullLocalPath + base_index;

		// if the default file doesnt exist and the auto index is off
		if (access(append_index.c_str(), F_OK) != 0 && !master_socket->get_rootLocation().autoindex)
		{
			if (errno == ENOENT)
				handler_response.statusCode = 404;  // Not Found
			else if (errno == EACCES)
				handler_response.statusCode = 403;  // Forbidden
			else
				handler_response.statusCode = 500;  // Internal Server Error
			handler_response.htmlContentType = "text/html";
			return;
		}

		// if the default file doesnt exist and the auto index is on
		else if (access(append_index.c_str(), F_OK) != 0 && master_socket->get_rootLocation().autoindex)
		{
			FetchHtmlBody::dispatchResponse(*this, fullLocalPath, "text/html");
		}

		// if the default file exists
		else
		{
			handler_response.htmlContentType = "text/html";
			std::string mimeType2 = getMimeType(append_index);
			FetchHtmlBody::dispatchResponse(*this, append_index, "text/html");
		}
	}

	// if we received a file
	else
	{
		std::string mimeType = getMimeType(fullLocalPath);
		if (mimeType == "what the fuck")
		{
			handler_response.htmlContentType = "text/html";
			handler_response.statusCode = 501; // todo: add 501 error page
		}
		else
			FetchHtmlBody::dispatchResponse(*this, fullLocalPath, mimeType);
	}
}

// Function to extract the file extension
std::string Handler::getFileExtension(const std::string& filename)
{
	size_t dotPos = filename.find_last_of(".");
	if (dotPos == std::string::npos) {
		return "";  // No extension found
	}
	return filename.substr(dotPos);
}


std::map<std::string, std::string> Handler::getMimeTypes()
 {
	std::map<std::string, std::string> mimeTypes;

	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".php"] = "text/html";

	return mimeTypes;
}

std::string Handler::getMimeType(const std::string& filePath)
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


void	Handler::parseRequest(ListeningSocket *master_socket, char **env)
{
	size_t		startPos;
	size_t		endPos

	size_t headersEndPos = _completeData.find("\r\n\r\n");
	if (headersEndPos == std::string::npos)
	{
		// TODO: error code: the final request received is incorrect
	}
	
	std::string headers = _completeData.substr(0, headersEndPos);
	std::string body = _completeData.substr(headersEndPos + 4);  // +4 to skip "\r\n\r\n"

	// lower case the headers to find "content-length" and "Content-Length"
	std::string lowerCaseHeaders = toLowerCase(headers);

	parseHeaders(master_socket, env, lowerCaseHeaders);
	if (_body_flag == true)
	{
		parseBody(body);
	}
	// todo: parse body
	// parseBody(master_socket, env, body);
}

int hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1; // invalid character
}

void Handler::parseBody(std::string body)
{
	std::vector<u_int8_t> baseBody(body.begin(), body.end());

	if (_chunk_flag == true)
	{

		size_t i = 0;
		while (i < baseBody.size())
		{
			// Parse the chunk size
			size_t chunkSize = 0;

			while (i < baseBody.size() && baseBody[i] != '\r')
			{
				chunkSize = chunkSize * 16 + std::hex_char_to_int(baseBody[i]); // You need to convert the char to its hexadecimal value
				i++;
			}
			i += 2; // Skip the CRLF after the chunk size

			// Append the chunk data to _bodyVector
			for (size_t j = 0; j < chunkSize; j++)
			{
				_bodyVector.push_back(baseBody[i + j]);
			}
			
			i += chunkSize + 2; // Skip the chunk data and the following CRLF
		}
	}
	else
	{
		_bodyVector = baseBody;
	}
	_bodyString.append((char*)_bodyVector.data(), _bodyVector.size());
	if (_bodyString.length() > _server.get_client_max_body_size())
	{
		set_response_status_code(413);
	}

}

void Handler::parseHeaders(ListeningSocket *master_socket, char **env, std::string headers)
{
	cgiEnvVector.clear();
	cgiEnv.clear();

	std::istringstream requestStream(headers);
	std::string line;

	if (std::getline(requestStream, line))  // 1. Request line
	{
		method = getStringPiece(line, 0);
		if (method == "get" || method == "post" || method == "delete")
			toUpperCase(method);
		else
		{
			// method not implemented
			set_response_status_code(405);
			return;
		}
		cgiEnvVector.push_back("REQUEST_METHOD=" + method);

		std::size_t questMarkPosition = getStringPiece(line, 1).find("?");
		cgiEnvVector.push_back("QUERY_STRING=" + (questMarkPosition == std::string::npos ? "" : getStringPiece(line, 1).substr(questMarkPosition +1)));
		cgiEnvVector.push_back("SERVER_PORT=" + std::to_string(master_socket->get_port()));
	
		path = getStringPiece(line, 1).substr(0, questMarkPosition);

		cgiEnvVector.push_back("PATH_INFO=" + path);
		cgiEnvVector.push_back("SCRIPT_INFO=" + path);
		cgiEnvVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
		// todo: check HTTP/1.1
	}

	while (*env) // Process environment variables (assuming env is some external char**)
	{
		std::string tmp(*env);
		if (tmp.substr(0, 5) == "PATH=")
		{
			cgiEnvVector.push_back(tmp);
			break;
		}
		env++;
	}

	while (std::getline(requestStream, line)) // Remaining lines (headers etc.)
	{
		if (getStringPiece(line, 0) == "connection:")
			connection = getStringPiece(line, 1);
		else if (getStringPiece(line, 0) == "transfer-encoding: chunked")
		{
			_body_flag = true;
			_chunk_flag = true;
		}
		else if (!getStringPiece(line, 0).compare("content-length:"))
		{
			try
			{
				contentLength = stoi(getStringPiece(line, 1));
				cgiEnvVector.push_back("CONTENT_LENGTH=" + getStringPiece(line, 1));
				_body_flag = true;
			}
			catch (const std::exception& e)
			{
				// TODO: Handle error
				// Maybe log an error message or set a default content length
			}
		}
	}

	for(unsigned int i = 0; i < cgiEnvVector.size(); i++)
	{
		cgiEnv.push_back(const_cast<char*>(cgiEnvVector[i].c_str()));;
		// std::cout <<"env string variable " << cgiEnvVector[i] << " " << std::endl;
		// std::cout <<"env const char *variable " << cgiEnv[i] << " " << std::endl;
	}
	// cgiEnv[index + 1] = NULL;
}








std::ostream &operator << (std::ostream &o, Handler  & handler )
{
	o << "Method: " << handler.get_method() << std::endl;
	o << "Path: " << handler.get_path() << std::endl;
	o << "content-length: " << handler.get_contentLength() << std::endl;
	o << "Connection: " << handler.get_connection() << std::endl;
	for(unsigned int i = 0; i < handler.get_cgiEnv().size(); i++)
	{
		std::cout <<"env const char *variable " << handler.get_cgiEnv()[i] << " " << std::endl;
	}

	return (o);

};


// GETTERS

void Handler::set_response_status_code(int code)
{
	this->handler_response.statusCode = code;
}

void Handler::set_response_keepAlive(bool res)
{
	this->handler_response.keepAlive = res;
}

void Handler::set_response_htmlContentType(std::string type)
{
	this->handler_response.htmlContentType = type;
}

void	Handler::set_response_htmlBody(std::string body)
{
	this->handler_response.htmlBody = body;
}

void	HttpRequest::setMaxBodySize(size_t size)
{
	_max_body_size = size;
}

char *Handler::get_buffer()
{
	return(this->_buffer);
}

std::string	Handler::get_completeData()
{
	return(this->_completeData);
}


std::string	Handler::get_method()
{
	return(this->method);
}

std::string	Handler::get_path()
{
	return(this->path);
}

std::string	Handler::get_fullLocalPath()
{
	return(this->fullLocalPath);
}

std::string	Handler::get_base_index()
{
	return(this->base_index);
}

std::string	Handler::get_query_string()
{
	return(this->query_string);
}

std::string	Handler::get_filename()
{
	return(this->filename);
}

std::string	Handler::get_connection()
{
	return(this->connection);
}

int	Handler::get_contentLength()
{
	return(this->contentLength);
}

std::vector<std::string>	Handler::get_cgiEnvVector()
{
	return(this->cgiEnvVector);
}

std::vector<char *>	Handler::get_cgiEnv()
{
	return(this->cgiEnv);
}


exec_info_t	Handler::get_exec_info()
{
	return(this->exec_info);
}

handler_response_t	Handler::get_handler_response()
{
	return(this->handler_response);
}

std::string &Handler::getBody()
{
	return (_bodyString);
}
