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
#include "Utils.hpp"

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
#include <fcntl.h>

#include <string>
#include <algorithm>
#include <cctype>

class WebServer : public AServer
{
	public:
		WebServer(char *config_file, char **env);
		~WebServer();


		void	launch();
		void	initializeSets();
		void	addToSet(const int i, fd_set &set);
		void	removeFromSet(const int i, fd_set &set);
		long	acceptNewConnection(MasterSocket &serv);
		int		readRequest(long socket, MasterSocket &serv);

		long	writeRequest(long socket, MasterSocket &serv);
		void	closeConnection(const int i);
		bool	requestCompletelyReceived(std::string completeData, MasterSocket &serv, long socket);

	private:
		// std::string	completeData; --> moved to client
		int			new_socket;
		char		**env;


		WebServer();
		// Handler handler;

};

#endif