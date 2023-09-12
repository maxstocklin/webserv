/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/12 15:08:47 by mstockli         ###   ########.fr       */
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

	if ((new_socket = accept(master_socket->get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		perror("Accept: ");
		exit(EXIT_FAILURE);
	}

	memset(buffer, 0, sizeof(buffer));

	std::cout << "Socket fd = " << master_socket->get_sock() << " and root loc = " << master_socket->get_rootLocation().root << "___________________________________\n\n\n";
	int bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
	if (bytes_read == -1)
	{
		perror("buff -1 3");
		exit(1);
	}
	buffer[bytes_read] = '\0'; // Null-terminate the buffer
}

void TestServer::handler(ListeningSocket *master_socket)
{
	std::cout << "###################### Buffer start ######################" << std::endl;
	std::cout << buffer << std::endl;
	// std::cout << "###################### Buffer end  ######################" << std::endl;
	// std::cout << "###################### HERE COMES THE PARSED RESULTS ######################" << std::endl;
	request.setBuffer(buffer);
	request.parse(master_socket);
	// std::cout << request << std::endl;

	// std::cout << "###################### End Parsed Results ######################" << std::endl;

}


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void TestServer::responder()
{
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
		}
	}


    int pipefd[2];
    pipe(pipefd);

	pid_t pid = fork();
	if (pid == 0) {

        close(pipefd[0]);
        int errdup = dup2(pipefd[1], STDOUT_FILENO);

		if (pipefd[1] == -1 || errdup == -1)
		{
			perror("error man");
			exit(0);
		}
		char *argv[] =
		{
			const_cast<char*>("/usr/bin/php"), 
			const_cast<char*>("/Users/mstockli/cursus/webss/Networking/Cgi/index.php"), 
			NULL
		};
		if (request.cgiEnv.empty() || request.cgiEnv.back() != nullptr)
		{
			request.cgiEnv.push_back(nullptr); 
		}

        execve("/usr/bin/php", argv, request.cgiEnv.data());
		perror("execve failed");

        exit(0);
	}
	else
	{
		waitpid(pid, NULL, 0);  // Wait for child to finish
		// Parent process code to read from pipe...
	}


    close(pipefd[1]);

    char html_content[4096];
	memset(html_content, 0, sizeof(html_content));

    int readbytes2 = read(pipefd[0], html_content, sizeof(html_content));
	if (readbytes2 == -1)
	{
		perror("buff -1 1");
		exit(1);
	}

	html_content[readbytes2] = 0;

	const char *response_headers = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n";

	char response[2048];

	int content_length = strlen(html_content);

	snprintf(response, sizeof(response), "%sContent-Length: %d\r\n\r\n%s", response_headers, content_length, html_content);
	std::cout << "---resp\n" << response << "----\n";

	if (write(new_socket, response, strlen(response)) == -1)
	{
		perror("buff -1 2");
		exit(1);
	}

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
				accepter(get_socket(i));
				handler(get_socket(i));
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
