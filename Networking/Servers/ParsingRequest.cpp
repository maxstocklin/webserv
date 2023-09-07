#include "../../Includes/ParsingRequest.hpp"


ParsingRequest::ParsingRequest(){

};
ParsingRequest::~ParsingRequest(){

};

void ParsingRequest::setBuffer(char * buffer) 
{
    strcpy(_buffer, buffer);
};

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

std::map<std::string, std::string> ParsingRequest::getParameters(std::string params){
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

void ParsingRequest::parse(){
    urlParams.clear();
    bodyParams.clear();
    char *ptr;
    ptr = strtok( _buffer, "\n");
    //method and path always on first line
    method = getStringPiece(ptr, 0);
    
    cgiEnvVector.push_back("REQUEST_METHOD: " + method);

    std::size_t questMarkPosition = getStringPiece(ptr, 1).find("?");
    urlParams = getParameters(getStringPiece(ptr, 1).substr(questMarkPosition +1));
    cgiEnvVector.push_back("QUERY_STRING: " + getStringPiece(ptr, 1).substr(questMarkPosition +1));

    path = getStringPiece(ptr, 1).substr(0, questMarkPosition);
    cgiEnvVector.push_back("PATH_INFO: " + getStringPiece(ptr, 1).substr(0, questMarkPosition));
    cgiEnvVector.push_back("SCRIPT_INFO: " + getStringPiece(ptr, 1).substr(0, questMarkPosition));
    cgiEnvVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
    while (ptr != NULL)  
    {  
        if (!getStringPiece(ptr, 0).compare("Connection:"))
            connection = getStringPiece(ptr, 1);
        if (!getStringPiece(ptr, 0).compare("content-length:"))
        {
            contentLength = stoi(getStringPiece(ptr, 1));
            cgiEnvVector.push_back("CONTENT_LENGTH: "+ getStringPiece(ptr, 1));
        }
        if (getStringPiece(ptr, 0).find_first_of(":") > getStringPiece(ptr, 0).size())
        {
           bodyParams =  getParameters(getStringPiece(ptr, 0));
        }

        ptr = strtok (NULL, "\n");  
    }  


}

std::ostream &operator << (std::ostream &o, ParsingRequest  & request )
{
    o << "Method: " << request.method << std::endl;
	o << "Path: " << request.path << std::endl;
	o << "content-length: " << request.contentLength << std::endl;
	o << "Connection: " << request.connection << std::endl;
	for (std::map<std::string, std::string>::iterator i = request.urlParams.begin(); i != request.urlParams.end(); i++ )
	{
		o << "urlParam: " << i->first << " : " << i->second << std::endl;
	}
	for (std::map<std::string, std::string>::iterator i = request.bodyParams.begin(); i != request.bodyParams.end(); i++ )
	{
		o << "bodyParam: " << i->first << " : " << i->second << std::endl;
	}


    return (o);

};