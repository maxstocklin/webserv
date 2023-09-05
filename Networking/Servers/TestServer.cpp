/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 01:22:10 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TestServer.hpp"
#include <cstring>
#include <iostream>



TestServer::TestServer() : Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
	launch();
}


void TestServer::accepter()
{
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof(address);
	new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	
	memset(buffer, 0, sizeof(buffer)); // Zero out the buffer
	/*
	1. Ensure that the server handles partial reads and writes if the data is large for one call
	--> getnextline()
	2. check for errors (i.e. if bytes_read = -1)
	*/
	int bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0'; // Null-terminate the buffer
}

// for now, it doesn't receive anything, this function will be used to parse the HTTP request
// error handling: invalid data in the HTTP request
// handle CGI scripts?
void TestServer::handler()
{
	std::cout << "THIS IS THE BUFFER BIATCH" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "THIS IS THE BUFFER BIATCH22" << std::endl;
}

void TestServer::responder()
{
	const char *response_headers = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n"
	"\r\n";

	const char *html_content = 
	"<!DOCTYPE html>\r\n"
	"<html>\r\n"
	"<head><title>My Page</title></head>\r\n"
	"<body>\r\n"
	"<h1>Hello, my name is Jeff!</h1>\r\n"
	"<p>I'm a web developer with a passion for learning new things.</p>\r\n"
	"<h2>Some of my hobbies:</h2>\r\n"
	"<ul>\r\n"
	"    <li>Coding</li>\r\n"
	"    <li>Photography</li>\r\n"
	"    <li>Traveling</li>\r\n"
	"</ul>\r\n"
	"</body>\r\n"
	"</html>";

	char response[2048]; // Make sure the size is enough to contain both headers and content
	sprintf(response, "%s%s", response_headers, html_content);

	// later, this function will, instead of 'response,' use the parsed buffer from the read function
	write(new_socket, response, strlen(response));
	
	/*
	AFTER THE PARSING:
	Depending on the HTTP headers (like Connection: keep-alive), 
	We might keep the connection open for further requests from the same client.
	*/
	close(new_socket);
}


void TestServer::launch()
{
	int count = 0;
	while (1)
	{
		std::cout << "============= WAITING =============" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "============= DONE =============" << std::endl;
		std::cout << "count = " << count << std::endl;
		count++;
	}
}
