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

void ParsingRequest::getUrlParameters(std::string params){
    int start = 0;
    int end = params.find("&");
    while (end != -1) {
        int equalPosition = params.substr(start, end - start).find("=");
        urlParams[params.substr(start, end - start).substr(0, equalPosition)] = params.substr(start, end - start).substr(equalPosition + 1);
        start = end + 1;
        end = params.find("&", start);
    }
    int equalPosition = params.substr(start).find_last_of("=");
    if (equalPosition > 0)
        urlParams[params.substr(start).substr(0, equalPosition)] = params.substr(start).substr(equalPosition + 1);
};

void ParsingRequest::getBodyParameters(std::string params){
    int start = 0;
    int end = params.find("&");
    while (end != -1) {
        int equalPosition = params.substr(start, end - start).find("=");
        bodyParams[params.substr(start, end - start).substr(0, equalPosition)] = params.substr(start, end - start).substr(equalPosition + 1);
        start = end + 1;
        end = params.find("&", start);
    }
    int equalPosition = params.substr(start).find_last_of("=");
    if (equalPosition > 0)
        bodyParams[params.substr(start).substr(0, equalPosition)] = params.substr(start).substr(equalPosition + 1);
};

void ParsingRequest::parse(){
    urlParams.clear();
    bodyParams.clear();
    char *ptr;
    ptr = strtok( _buffer, "\n");
    //method and path always on first line
    method = getStringPiece(ptr, 0);
    std::size_t questMarkPosition = getStringPiece(ptr, 1).find("?");
    getUrlParameters(getStringPiece(ptr, 1).substr(questMarkPosition +1));

    path = getStringPiece(ptr, 1).substr(0, questMarkPosition);
    while (ptr != NULL)  
    {  
        if (!getStringPiece(ptr, 0).compare("Connection:"))
            connection = getStringPiece(ptr, 1);
        if (!getStringPiece(ptr, 0).compare("content-length:"))
            contentLength = stoi(getStringPiece(ptr, 1));
        if (getStringPiece(ptr, 0).find_first_of(":") > getStringPiece(ptr, 0).size())
        {
            getBodyParameters(getStringPiece(ptr, 0));
        }

        ptr = strtok (NULL, "\n");  
    }  


}