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

#include "AServer.hpp"
#include "Colors.hpp"
#include "Handler.hpp"
#include "Responder.hpp"

#include <stdio.h>
#include <iostream>
#include <cstring>

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

class WebServer : public AServer
{
	public:
		WebServer(char *config_file);
		WebServer(char *config_file, char **env);
		~WebServer();
		void launch();

	private:
		std::string completeData;
		char buffer[1024];
		int new_socket;

		WebServer();
		void accepter(ListeningSocket *master_socket);
		void handle(ListeningSocket *master_socket);
		void responder(ListeningSocket *master_socket);
		char **env;
		Handler handler;

};

#endif