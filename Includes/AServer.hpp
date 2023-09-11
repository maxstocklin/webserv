/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/10 23:09:25 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASEVER_HPP
# define ASEVER_HPP

#include "ListeningSocket.hpp"
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
		virtual ~AServer();
		void split_server_blocks(const char *config_file);
		virtual void launch() = 0;

		// getters
		ListeningSocket * get_socket(int i);
		std::vector<ListeningSocket*> get_socket_vector();
		
	private:
		AServer();

		std::vector<ListeningSocket*> sockets;
		std::vector<std::string> serverBlocks;

		std::string removeCommentLines(const std::string &input);

		virtual void accepter(ListeningSocket *master_socket) = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;
};

#endif