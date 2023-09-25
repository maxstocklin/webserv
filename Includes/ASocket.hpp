/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/24 23:43:20 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASOCKET_HPP
# define ASOCKET_HPP

#include "Colors.hpp"
#include "ServerConfig.hpp"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <string>

class ASocket
{
	public:
		// constructors
		ASocket(std::string serverBlock);
		virtual ~ASocket();

		// virtual function to connect to network
		virtual int					connect_network(int sock, struct sockaddr_in address) = 0;

		// test sockets and connections
		void						test_connection(int item_to_test);

		// PARSING
		void						parseServerBlock(std::string serverBlock);
		bool						containsDirective(const std::string &serverBlock, const std::string &directive);
		std::string					removeDirective(std::string &serverBlock, const std::string &directive);

		// extract directives
		int							extractPort(std::string line);
		std::string					extractHost(std::string line);
		std::string					extractServer_name(std::string line);
		void						extractError_page(std::string line);
		int							extractClient_max_body_size(std::string line);


		// extract locations
		void						parseLocations(std::string &serverBlock);
		void						parseRootLocation(std::string &serverBlock);
		struct Location				createLocation(std::string locStr);
		std::string					extractRoot(std::string line);
		bool						extractAutoindex(std::string line);
		std::vector<std::string>	extractAllow_methods(std::string line);
		std::string					extractIndex(std::string line, std::string path);




		// getters
		struct sockaddr_in			get_address();
		int							get_sock();
		int							get_connection();
		int							get_port();
		int							get_domain();
		std::string					get_host();
		int							get_host_int();		// todo: not hardcode
		std::string					get_server_name();
		std::string					get_index();
		std::map<int, std::string>	get_error_pages();
		int							get_client_max_body_size();
		Location					get_rootLocation();
		std::vector<Location>		get_locations();
		std::string					get_endpoint();
		ServerConfig				*get_config();
		int							get_service();
		u_long						get_interface();
		int							get_protocol();

		// setters
		void						set_connection(int con);


	private:
		ASocket();

		// main socket's fd
		int							sock_fd;

		int							connection;		
		struct sockaddr_in			address;

		ServerConfig 				*config;

};



#endif
