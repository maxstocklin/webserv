/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/15 11:09:58 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HANDLER_HPP
# define HANDLER_HPP

#include <map>
#include <vector>
#include <iostream>
#include "ListeningSocket.hpp"
#include "FetchHtmlBody.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <algorithm>

struct exec_info_t {
	std::string path ;
	bool found;
};

struct handler_response_t {
	int statusCode;
	std::string htmlBody;
	bool keepAlive;
	std::string htmlContentType;
};

class Handler
{

	public:
		// constructors
		Handler();
		~Handler();

		// helper methods
		void								parse(ListeningSocket *master_socket, char **env);
		void								setBuffer(std::string completeData);
		std::map<std::string, std::string>	getParameters(std::string params);

		void								getExecutablePath(std::string command);
		void								makeFullLocalPath(ListeningSocket *master_socket);
		void								getPathResponse(ListeningSocket *master_socket, int new_socket);
		std::string							getFileExtension(const std::string& filename);
		std::string							getMimeType(const std::string& filePath);
		std::map<std::string, std::string>	getMimeTypes();

		bool								isDirectory(const std::string& path);
		bool								isFile(const std::string& path);

		char						_buffer[30000];
		std::string					_completeData;

		std::string					method;
		std::string					path;
		std::string					fullLocalPath;
		std::string					base_index;
		std::string					query_string;
		std::string					filename;
		std::string					connection;
		int contentLength;
		std::vector<std::string>	cgiEnvVector;
		std::vector<char *>			cgiEnv;

		exec_info_t					exec_info;
		handler_response_t			handler_response;

	private:


};

std::ostream &operator << (std::ostream &o, Handler  & pr );


#endif