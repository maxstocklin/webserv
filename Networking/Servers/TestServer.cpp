/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 21:02:17 by mstockli         ###   ########.fr       */
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
	int bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0'; // Null-terminate the buffer
}

void TestServer::handler()
{
	std::cout << buffer << std::endl;
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

	write(new_socket, response, strlen(response));
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
