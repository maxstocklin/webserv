/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/14 03:02:32 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#define MAX_CLIENTS 5

#include "AServer.hpp"
#include "Colors.hpp"
#include "Handler.hpp"
#include "Responder.hpp"
#include "Client.hpp"

#include <stdio.h>
#include <iostream>
#include <cstring>

#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>

class WebServer : public AServer
{
	public:
		WebServer(char *config_file, char **env);
		~WebServer();


		void	launch();
		void	initializeSets();
		void	addToSet(const int i, fd_set &set);
		void	removeFromSet(const int i, fd_set &set);
		void	acceptNewConnection(ListeningSocket &serv);
		void	readRequest(const int &i, Client &c);
		void	closeConnection(const int i);
		bool	requestCompletelyReceived(std::string completeData);
		std::string	trimWhiteSpaces(const std::string &str);
	private:
		// std::string	completeData; --> moved to client
		int			new_socket;

		fd_set		_recv_fd_pool;	// receiving new sockets
		fd_set		_write_fd_pool;	// writing new sockets
		int			_biggest_fd;	// biggest actual socket's fd


		WebServer();
		void accepter(ListeningSocket *master_socket);
		void handle(ListeningSocket *master_socket);
		void responder(ListeningSocket *master_socket);
		char **env;
		// Handler handler;

};

#endif