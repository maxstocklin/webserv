/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/10/03 20:25:23 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASEVER_HPP
# define ASEVER_HPP

#include "MasterSocket.hpp"
#include "Colors.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <sstream>

class AServer
{
	public:
		AServer(char *config_file);
		virtual	~AServer();

		void							split_server_blocks(const char *config_file);
		virtual void					launch() = 0;

		// GETTERS
		MasterSocket					get_socket(int i);
		std::vector<MasterSocket>		get_socket_vector();
		
	protected:
		AServer();

		std::vector<std::string>		serverBlocks;	// a vector of all the server {} block for config
		std::vector<MasterSocket>		sockets;		// a vector of all the master sockets


		std::map<long, MasterSocket>	_servers;		// all master sockets / servers
		std::map<long, MasterSocket *>	_sockets;		// all new sockets
		std::vector<int>				_ready;			// a vector of all the ready new sockets
		fd_set							_fd_set;
		long							_max_fd;






		std::string						removeCommentLines(const std::string &input);

};

#endif