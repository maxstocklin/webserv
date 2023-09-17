/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responder.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:30:27 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/17 18:25:18 by max              ###   ########.fr       */
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
	statusMessage[500] = "Internal Server Error";  		// The server encountered an unexpected condition that prevented it from fulfilling the request.}
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
	std::string headers = response_headers + "Content-Length: " + std::to_string(handler.get_handler_response().htmlBody.size()) + "\r\n\r\n";

	// 2. Combine headers and content
	std::string fullResponse = headers + handler.get_handler_response().htmlBody;

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