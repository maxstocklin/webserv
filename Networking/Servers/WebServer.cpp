/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/15 17:24:31 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/WebServer.hpp"
#include "../../Includes/FetchHtmlBody.hpp"
#include "../../Includes/Handler.hpp"


WebServer::WebServer(char *config_file) : AServer(config_file)
{
	memset(buffer, 0, sizeof(buffer));
	launch();
}

WebServer::WebServer(char *config_file, char **env) : AServer(config_file), env(env)
{
	memset(buffer, 0, sizeof(buffer));
	launch();
}

WebServer::~WebServer()
{
	
}

void WebServer::accepter(ListeningSocket *master_socket)
{
	struct sockaddr_in address = master_socket->get_address();
	int addrlen = sizeof(address);

	if ((new_socket = accept(master_socket->get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		const char* errorMessage = strerror(errno);  // Retrieve human-readable error message

		if (errno == ENOENT)
			handler.handler_response.statusCode = 404;  // Not Found
		else if (errno == EACCES)
			handler.handler_response.statusCode = 403;  // Forbidden
		else
			handler.handler_response.statusCode = 500;  // Internal Server Error
		handler.handler_response.htmlContentType = "text/html";
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	completeData.clear();
	while (true)
	{
		ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);


		if (bytes_read > 0)
		{
			completeData.append(buffer, bytes_read);
			if (completeData.find("\r\n\r\n") != std::string::npos)
				break;

		}
		else if (bytes_read == 0)
		{
			break;
		}
		else
		{
			// Handle errors based on errno
			if (errno == ECONNRESET) {
				// Connection reset by peer
				// Handle this specific error, e.g., log it and then break out of loop
				std::cerr << "Connection reset by peer." << std::endl;
				break;
			}
	
			handler.handler_response.statusCode = 500;  // Internal Server Error
			handler.handler_response.htmlContentType = "text/html";
			return;
		}


	}
}

void WebServer::handle(ListeningSocket *master_socket)
{
	std::cout << "###################### HTTP REQUEST ######################" << std::endl  << std::endl;
	std::cout << buffer << std::endl;

	handler.setBuffer(completeData);
	handler.parse(master_socket, env);
	// std::cout << handler << std::endl;

	handler.makeFullLocalPath(master_socket);
	handler.getPathResponse(master_socket, new_socket);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void WebServer::responder(ListeningSocket *master_socket)
{
	Responder resp(handler, master_socket->get_error_pages(), new_socket);
}


void WebServer::launch()
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
		client_socket[i] = 0;

	int count = 0;
	while (1)
	{
		FD_ZERO(&readfds);
		max_sd = -1;

		for (size_t i = 0; i < get_socket_vector().size(); i++)
		{
			int socket_fd = get_socket(i)->get_sock();
			FD_SET(socket_fd, &readfds); // Add each master socket FD to the set
			
			if (socket_fd > max_sd)
				max_sd = socket_fd;
		}

		for (int j = 0 ; j < max_clients ; j++)
		{
			sd = client_socket[j];
			if(sd > 0)
				FD_SET(sd, &readfds);
			if(sd > max_sd)
				max_sd = sd;
		}

		std::cout << "============= WAITING FOR NEXT CONNECT =============" << std::endl;

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

			if (FD_ISSET(master_socket_fd, &readfds))
			{
				handler.handler_response.statusCode = 0;
				accepter(get_socket(i));
				handle(get_socket(i));
				responder(get_socket(i));
				std::cout << "count = " << count << std::endl;
				std::cout << "============= DONE =============" << std::endl;
				count++;

				if (handler.connection == "keep-alive")
				{
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
				else
				{
					close(new_socket);
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
				memset(buffer, 0, sizeof(buffer));
				if ((valread = read( sd , buffer, sizeof(buffer))) == 0)
				{
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
