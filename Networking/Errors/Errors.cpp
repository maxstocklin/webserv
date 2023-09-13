/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:30:27 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/13 12:55:24 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Errors.hpp"

Errors::Errors(std::map<int, std::string> errorMap) : errorMap(errorMap)
{
	statusMessage[200] = "OK";							// The server cannot or will not process the request due to something that is perceived to be a client error (e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
	statusMessage[400] = "Bad Request";					// The server cannot or will not process the request due to something that is perceived to be a client error (e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
	statusMessage[403] = "Forbidden";  					// The server understood the request, but it refuses to fulfill it. Authorization will not help and the request should not be repeated.
	statusMessage[404] = "Not Found";  					// The server can't find the requested resource. Links which lead to a 404 page are often called broken or dead links.
	statusMessage[405] = "Method Not Allowed"; 			// The request method is known by the server but is not supported by the target resource.
	statusMessage[410] = "Gone";   						// This response is sent when the requested content has been permanently deleted from the server, with no forwarding address.
	statusMessage[413] = "Payload Too Large";  			// The request is larger than the server is willing or able to process.
	statusMessage[500] = "Internal Server Error";  		// The server encountered an unexpected condition that prevented it from fulfilling the request.}

Errors::~Errors()
{

}

struct  responseStruct
{
	int statusCode;
	bool keepAlive;
	std::string html_content;
	std::string contentType;
	
}

void responder(const responseStruct& response)
{
	char html_content[20048];
	
	std::string response_headers;

	response_headers = createResponseHeader(response);



	const char *response_headers = 
	"HTTP/1.1 404 OK\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n";

	char response[20048];

	int content_length = strlen(html_content);

	snprintf(response, sizeof(response), "%sContent-Length: %d\r\n\r\n%s", response_headers, content_length, html_content);
	std::cout << "---resp\n" << response << "----\n";

	if (write(new_socket, response, strlen(response)) == -1)
	{
		perror("buff -1 2");
		exit(1);
	}


}
}


std::string createResponseHeader(const responseStruct& response)
{
	std::string response_headers;

	// Add the HTTP version and status code
	response_headers += "HTTP/1.1 ";

	if (statusMessage.find(response.statusCode) != statusMessage.end())
		response_headers += std::to_string(response.statusCode) + " " + statusMessage[response.statusCode];
	else
		response_headers += std::to_string(404) + " " + statusMessage[404]; // Default to 404 for unkown codes

	response_headers += "\r\n";

	// Add the Content-Type
	response_headers += "Content-Type: " + response.contentType + "\r\n";

	// Add the keepAlive flag
	if (response.keepAlive)
		response_headers += "Connection: keep-alive\r\n";
	else
		response_headers += "Connection: close\r\n";

	return (response_headers);

}






std::string Errors::get_error_content(int statusCode)
{
	std::string errorPageContent;

	if (errorMap.find(statusCode) != errorMap.end())
	{
		// Load the custom error page from the file
		errorPageContent = loadFile(errorMap[statusCode]);
	}
	else
	{
		// Load the default error page
		errorPageContent = loadFile(errorMap[0]);
	}

	if (errorPageContent.empty())
	{
		throw std::runtime_error("ERROR: Error with error file.");
	}

	// sendResponse(statusCode, errorPageContent);

}


std::string loadFile(std::string errorFile)
{
	int op;
	int readbytes;
	char buffer[2000];

	memset(buffer, 0, sizeof(buffer));
	
	if (op = open(errorFile.c_str(), O_RDONLY) < 0)
		throw std::runtime_error("ERROR: Error with error file.");
	if (readbytes = read(op, buffer, sizeof(buffer)) < 0)
		throw std::runtime_error("ERROR: Error with error file.");
	
	buffer[readbytes] = 0;

	return (buffer);
	
}