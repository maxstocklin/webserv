/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 16:45:37 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TestServer.hpp"
#include <cstring>
#include <iostream>

#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

	
#define TRUE 1
#define FALSE 0

TestServer::TestServer() : Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 3)
{
	launch();
}


void TestServer::accepter()
{
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof(address);

	//get_socket() returns the main socket instance (ListenSocket class)
	//get_sock() returns the main socket FD
	/*new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);*/
	
	// added
	if ((new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	{
		perror("Accept:");
		exit(EXIT_FAILURE);
	}

	//inform user of socket number - used in send and receive commands
	printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
		(address.sin_port));

	
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
	std::cout << "HERE COMES THE BUFFER" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "THERE LEAVES THE BUFFER" << std::endl;
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
	//set of socket descriptors
	fd_set readfds;

	int client_socket[30];
	int max_clients = 30;
	int max_sd;
	int sd;
	int activity;
	int valread;
	int i;

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	
	int count = 0;
	while (1)
	{
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(get_socket()->get_sock(), &readfds);
		max_sd = get_socket()->get_sock();

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		std::cout << "============= WAITING =============" << std::endl;

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(get_socket()->get_sock(), &readfds))
		{
			accepter();
			sleep(2);
			handler();
			responder();
			std::cout << "============= DONE =============" << std::endl;
			std::cout << "count = " << count << std::endl;
			count++;

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}

		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}




		
	}
	// close(server_fd);
}
