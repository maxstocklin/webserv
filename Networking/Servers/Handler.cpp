#include "../../Includes/Handler.hpp"


Handler::Handler()
{
	handler_response.statusCode = 0;
	
}

Handler::~Handler()
{

}

void Handler::setBuffer(char * buffer) 
{
	strcpy(_buffer, buffer);
}

std::string getStringPiece(std::string line, int part)
{
  
	int count = 0;
	int start = 0;
	int end = line.find(" ");
	while (end != -1) {
		if (count == part)
			   return (line.substr(start, end - start));
		start = end + 1;
		end = line.find(" ", start);
		count++;
	}
	return (line.substr(start, end - start));
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
		// std::cout << testPath <<  std::endl;
		start = end + 1;
		struct stat sb;
		if ((stat(testPath.c_str(), &sb) == 0 && (sb.st_mode & S_IXUSR)))
		{
			// std::cout <<" test path " << testPath << " executable " << (stat(testPath.c_str(), &sb) == 0 && (sb.st_mode & S_IXUSR)) <<  std::endl;
			exec_info.found = true;
			exec_info.path = testPath.c_str();
		}
	}
}

void Handler::makeFullLocalPath(ListeningSocket *master_socket) {
	std::vector<Location> locationVector = master_socket->get_locations();

	std::cout << "request path : " << path << std::endl;
	
	std::string basePath;
	int secondBackslashPos = path.substr(1).find("/");
	if (secondBackslashPos != -1 || (secondBackslashPos == -1 && path.size() > 1))
	{
		basePath = path.substr(1, secondBackslashPos);
		std::cout << "basepath " << basePath << std::endl;
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
			int meth = 0;
			for (size_t j = 0; j < locationVector[i].allow_methods.size(); j++)
			{
				if (locationVector[i].allow_methods[j] == method)
					meth = 1;
			}
			if (meth == 0)
			{
				handler_response.statusCode = 405;
			}
			

			std::string first = locationVector[i].root;
			std::string sec = path.substr(basePath.length() + 1);
			
			if (!first.empty() && first[first.size() - 1] == '/' && !sec.empty() && sec[0] == '/')
				first.pop_back();

			// add a "/" if neither the first nor the sec have one
			else if (!first.empty() && first[first.size() - 1] != '/' && !sec.empty() && sec[0] != '/')
				first = first + '/';

			fullLocalPath = first + sec;
			std::cout << " full locationPath " << fullLocalPath << std::endl;
			 base_index = locationVector[i].index;
		}
		std::cout << locationVector[i].route << " " << locationVector[i].root << std::endl;
	}
	if (found_loc == false)
	{
			std::string first = master_socket->get_rootLocation().root;

			int meth = 0;
			for (size_t j = 0; j < master_socket->get_rootLocation().allow_methods.size(); j++)
			{
				if (master_socket->get_rootLocation().allow_methods[j] == method)
					meth = 1;
			}
			if (meth == 0)
			{
				handler_response.statusCode = 405;
			}
			if (!first.empty() && first[first.size() - 1] == '/' && !path.empty() && path[0] == '/')
				first.pop_back();

			// add a "/" if neither the first nor the path have one
			else if (!first.empty() && first[first.size() - 1] != '/' && !path.empty() && path[0] != '/')
				first = first + '/';

			fullLocalPath = first + path;
			std::cout << " full locationPath " << fullLocalPath << std::endl;
			 base_index = master_socket->get_rootLocation().index;

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
void Handler::getPathResponse(ListeningSocket *master_socket, int new_socket)
{

	std::string pathToCheck;

	(void)new_socket;
	// TODO: create a checker function
	if (handler_response.statusCode != 0)
	{
		handler_response.htmlContentType = "text/html";
		return;
	}
	if (access(fullLocalPath.c_str(), F_OK) != 0)
	{
		handler_response.htmlContentType = "text/html";
		handler_response.statusCode = 404;
		return;
	}
	std::cout << "\n\n\n DIR? \n\n\n" << fullLocalPath << "\n\n\n\n\n";
	if (isDirectory(fullLocalPath))
		std::cout << fullLocalPath << " is a directory." << std::endl;
	else if (isFile(fullLocalPath))
		std::cout << fullLocalPath << " is a file." << std::endl;
	else
		std::cout << "Unknown type or does not exist." << std::endl;

	if (!isFile(fullLocalPath))
	{
		std::cout << "appending index.html" << std::endl;
		if (fullLocalPath[fullLocalPath.size() - 1] != '/')
			fullLocalPath += "/";
		std::string append_index = fullLocalPath + base_index;

		// if the default file doesnt exist and the auto index is off
		if (access(append_index.c_str(), F_OK) != 0 && !master_socket->get_rootLocation().autoindex)
		   handler_response.statusCode = 404;

		// if the default file doesnt exist and the auto index is on
		else if (access(append_index.c_str(), F_OK) != 0 && master_socket->get_rootLocation().autoindex)
			FetchHtmlBody::dispatchResponse(*this, fullLocalPath, "none");

		// if the default file exists
		else
			FetchHtmlBody::dispatchResponse(*this, append_index, "none");

	}
	// if we received a file
	else
	{
		std::string mimeType = getMimeType(fullLocalPath);
		std::cout << "\n\n\n mimeType \n\n\n" << mimeType << "\n\n\n\n\n";
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


void Handler::parse(ListeningSocket *master_socket, char **env)
{

	cgiEnvVector.clear();
	cgiEnv.clear();
	char *ptr;
	ptr = strtok( _buffer, "\n");
	//method and path always on first line
	method = getStringPiece(ptr, 0);
	
	cgiEnvVector.push_back("REQUEST_METHOD=" + method);

	std::size_t questMarkPosition = getStringPiece(ptr, 1).find("?");
   
	cgiEnvVector.push_back("QUERY_STRING=" + (questMarkPosition == std::string::npos ? "" : getStringPiece(ptr, 1).substr(questMarkPosition +1)) );
	cgiEnvVector.push_back("SERVER_PORT=" + std::to_string((master_socket->get_port())));
	std::string tmp;
	while (*env)
	{
		tmp = (*env);
		if (!strncmp(*env, "PATH=", 5))
			cgiEnvVector.push_back(tmp);
		env++;
	}

	(void)env;

//    (void)master_socket;
	path = getStringPiece(ptr, 1).substr(0, questMarkPosition);
	cgiEnvVector.push_back("PATH_INFO=" + getStringPiece(ptr, 1).substr(0, questMarkPosition));
	cgiEnvVector.push_back("SCRIPT_INFO=" + getStringPiece(ptr, 1).substr(0, questMarkPosition));
	cgiEnvVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	while (ptr != NULL)
	{
		if (!getStringPiece(ptr, 0).compare("Connection:"))
			connection = getStringPiece(ptr, 1);
		if (!getStringPiece(ptr, 0).compare("content-length:"))
		{
			contentLength = stoi(getStringPiece(ptr, 1));
			cgiEnvVector.push_back("CONTENT_LENGTH="+ getStringPiece(ptr, 1));
		}
		ptr = strtok (NULL, "\n");  
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
	o << "Method: " << handler.method << std::endl;
	o << "Path: " << handler.path << std::endl;
	o << "content-length: " << handler.contentLength << std::endl;
	o << "Connection: " << handler.connection << std::endl;
	for(unsigned int i = 0; i < handler.cgiEnv.size(); i++)
	{
		std::cout <<"env const char *variable " << handler.cgiEnv[i] << " " << std::endl;
	}

	


	return (o);

};