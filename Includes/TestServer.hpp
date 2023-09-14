/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/14 03:02:32 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTSEVER_HPP
# define TESTSEVER_HPP

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

class TestServer : public AServer
{
	public:
		TestServer(char *config_file);
		TestServer(char *config_file, char **env);
		~TestServer();
		void launch();

	private:
		char buffer[30000];
		int new_socket;

		TestServer();
		void accepter(ListeningSocket *master_socket);
		void handle(ListeningSocket *master_socket);
		void responder(ListeningSocket *master_socket);
		char **env;
		Handler handler;

};

#endif