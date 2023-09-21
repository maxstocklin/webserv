/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/20 15:31:45 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASEVER_HPP
# define ASEVER_HPP

#include "ListeningSocket.hpp"
#include "Colors.hpp"
#include "Client.hpp"

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
		ListeningSocket					*get_socket(int i);
		std::vector<ListeningSocket>	get_socket_vector();
		
	private:
		AServer();

		std::vector<ListeningSocket>	sockets;		// a vector of all the master sockets
		std::vector<std::string>		serverBlocks;	// a vector of all the server {} block for config
		std::map<int, ListeningSocket>	_servers_map;	// a map of all the master sockets
		std::map<int, Client>			_clients_map;	// a map of all clients, for each new connections

		std::string						removeCommentLines(const std::string &input);

		virtual void					accepter(ListeningSocket	*master_socket) = 0;
		virtual void					handle(ListeningSocket *master_socket) = 0;
		virtual void					responder(ListeningSocket *master_socket) = 0;
};

#endif