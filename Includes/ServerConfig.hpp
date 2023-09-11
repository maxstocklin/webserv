/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:47:04 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/10 23:03:54 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "Colors.hpp"


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


struct Location
{
	std::string					route;			// URL path (or name)
	std::string					root;			// physical path
	std::string					index;			// default file
	std::vector<std::string>	allow_methods;	// GET, POST, DELETE
	bool						autoindex;		// auto look for path
};

class ServerConfig
{
	public:
		// constructors
		ServerConfig(std::string serverBlock);
		~ServerConfig();


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
		int							get_port();
		int							get_domain();
		std::string					get_host();
		std::string					get_server_name();
		std::string					get_index();
		std::map<int, std::string>	get_error_pages();
		int							get_client_max_body_size();
		Location					get_rootLocation();
		std::vector<Location>		get_locations();
		std::string					get_endpoint();
		int							get_service();
		u_long						get_interface();
		int							get_protocol();

		// Utils
		bool						isNumber(const std::string &s);
		bool						isValidPort(int port);
		bool						endsWithSemicolon(const std::string &str);
		bool						isValidHost(const std::string& host);
		std::string					trimWhiteSpaces(const std::string &str);
		std::vector<std::string>	splitToVector(const std::string &str);



	private:

		int							port;							// Default to 80
		int							domain;							// Default to AF_INET ??
		std::string					host;							// Default to 127.0.0.1
		std::string					server_name;					// Default to localhost
		std::string					index;							// Default to index.html
		std::map<int, std::string>	error_pages;
		int							client_max_body_size;			// Default to 1MB

		int							service;						// hardcoded
		int							protocol;						// hardcoded
		u_long						interface;						// hardcoded

		Location					rootLocation;					// Root or default location
		std::vector<Location>		locations;		

};

#endif
