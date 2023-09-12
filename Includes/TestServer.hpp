/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/12 16:16:56 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTSEVER_HPP
# define TESTSEVER_HPP

#include "AServer.hpp"
#include "Colors.hpp"
#include "ParsingRequest.hpp"

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
		void handler(ListeningSocket *master_socket);
		void responder(ListeningSocket *master_socket);
		char **env;
		ParsingRequest request;

};

#endif