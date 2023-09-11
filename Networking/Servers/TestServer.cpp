/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/11 11:55:38 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/TestServer.hpp"


TestServer::TestServer(char *config_file) : AServer(config_file)
{
	memset(buffer, 0, sizeof(buffer));
	// TESTING: ADD back when testing the whole program
	launch();
}

TestServer::~TestServer()
{
	
}

void TestServer::accepter(ListeningSocket *master_socket)
{
	struct sockaddr_in address = master_socket->get_address();
	int addrlen = sizeof(address);

	//master_socket returns the main socket instance (ListenSocket class)
	//get_sock() returns the main socket FD
	/*new_socket = accept(master_socket->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);*/
	if ((new_socket = accept(master_socket->get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	{
		perror("Accept:");
		exit(EXIT_FAILURE);
	}

	//inform user of socket number - used in send and receive commands
	printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
		(address.sin_port));

	
	memset(buffer, 0, sizeof(buffer));

	/*
	1. Ensure that the server handles partial reads and writes if the data is large for one call
	--> getnextline()
	2. check for errors (i.e. if bytes_read = -1)
	*/
	std::cout << "Socket fd = " << master_socket->get_sock() << " and root loc = " << master_socket->get_rootLocation().root << "___________________________________\n\n\n";
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
	char **av;
	av = (char **)malloc(10 * sizeof( char *));
	av[0] =(char*)malloc(sizeof(char) * 10);
	av[1] =(char*)malloc(sizeof(char) * 10);
	av[2] =(char*)malloc(sizeof(char) * 10);
	strcpy(av[0], "ls");
	strcpy(av[1], "-a");
	av[2] = 0;
	if (request.path.size() > 5 )
	{
		if (!(request.path.substr(request.path.size()-4, request.path.size()).compare(".php")))
		{	
			std::cout << "PHP Script found" << std::endl;
			int i = 0;
			while (env[i])
			{
				std::cout << env[i] << std::endl;
				i++;
			}
			execve("/bin/ls", av, env);
		}
	}

	const char *response_headers = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n";

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
	// sprintf(response, "%s%s", response_headers, html_content);
	int content_length = strlen(html_content);
	snprintf(response, sizeof(response), "%sContent-Length: %d\r\n\r\n%s", response_headers, content_length, html_content);

	// later, this function will, instead of 'response,' use the parsed buffer from the read function
	write(new_socket, response, strlen(response));
}


void TestServer::launch()
{
	//set of socket descriptors
	fd_set readfds;
	int client_socket[5];
	int max_clients = 5;
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

		// TODO: ADDED
		max_sd = -1;

		// TODO: ADDED
		for (size_t i = 0; i < get_socket_vector().size(); i++)
		{
			int socket_fd = get_socket(i)->get_sock();
			FD_SET(socket_fd, &readfds); // Add each master socket FD to the set
			
			if (socket_fd > max_sd)
				max_sd = socket_fd;
		}

		// TODO: REMOVED
		// //add master socket to set
		// FD_SET(get_socket()->get_sock(), &readfds);
		// max_sd = get_socket()->get_sock();

		//add child sockets to set
		for (int j = 0 ; j < max_clients ; j++)
		{
			//socket descriptor
			sd = client_socket[j];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET(sd, &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		std::cout << "============= WAITING FOR NEXT CONNECT =============" << std::endl;

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		printf("-------------------------------\nMAX SD = %d\n\n-------------------------------\n\n", max_sd);
		
		activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error && activity = %d\n\n", activity);
			perror("select");
		}
		// Check activity on each master socket
		for (size_t i = 0; i < get_socket_vector().size(); i++)
		{
			int master_socket_fd = get_socket(i)->get_sock();

			//If something happened on a master socket,
			//then its an incoming connection
			if (FD_ISSET(master_socket_fd, &readfds))
			{
				accepter(get_socket(i));
				sleep(2);
				handler();
				responder();
				std::cout << "count = " << count << std::endl;
				std::cout << "============= DONE =============" << std::endl;
				count++;

				//add new socket to array of sockets
				for (int j = 0; j < max_clients; j++)
				{
					//if position is empty
					if( client_socket[j] == 0 )
					{
						client_socket[j] = new_socket;
						printf("Adding to list of sockets as %d\n" , j);
						break;
					}
				}
			}
		}
		for (int j = 0; j < max_clients; j++)
		{
			sd = client_socket[j];

			if (FD_ISSET(sd, &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					/*
						AFTER THE PARSING:
						Depending on the HTTP headers (like Connection: keep-alive), 
						We might keep the connection open for further requests from the same client.
					*/

					// close(new_socket); --> ??

					//Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[j] = 0;
					std::cout << "this is closed" << std::endl;
				}

				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
					std::cout << "this is open" << std::endl;
				}
			}
		}	
	}
	// close(server_fd);
}
