/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/20 20:27:51 by max              ###   ########.fr       */
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
		void								parseHeaders(ListeningSocket *master_socket, char **env, std::string headers);
		void								parseRequest(ListeningSocket *master_socket, char **env);
		void								setBuffer(std::string completeData);
		std::map<std::string, std::string>	getParameters(std::string params);

		std::string							&getBody();

		void								getExecutablePath(std::string command);
		void								makeFullLocalPath(ListeningSocket *master_socket);
		void								getPathResponse(ListeningSocket *master_socket);
		std::string							getFileExtension(const std::string& filename);
		std::string							getMimeType(const std::string& filePath);
		std::map<std::string, std::string>	getMimeTypes();

		bool								isDirectory(const std::string& path);
		bool								isFile(const std::string& path);

		// SETTERS
		void								set_response_status_code(int code);
		void								set_response_keepAlive(bool res);
		void								set_response_htmlContentType(std::string type);
		void								set_response_htmlBody(std::string body);

		void								setMaxBodySize(size_t);

		// GETTERS
		char								*get_buffer();
		std::string							get_completeData();

		std::string							get_method();
		std::string							get_path();
		std::string							get_fullLocalPath();
		std::string							get_base_index();
		std::string							get_query_string();
		std::string							get_filename();
		std::string							get_connection();
		int									get_contentLength();
		std::vector<std::string>			get_cgiEnvVector();
		std::vector<char *>					get_cgiEnv();

		exec_info_t							get_exec_info();
		handler_response_t					get_handler_response();

	private:
		// char								_buffer[30000];
		// std::string							_completeData;

		std::string							method;
		std::string							path;
		std::string							fullLocalPath;
		std::string							base_index;
		std::string							query_string;
		std::string							filename;
		std::string							connection;
		int									contentLength;

		bool								_chunk_flag;
		bool								_body_flag;
		std::vector<u_int8_t>				_bodyVector;
		std::string							_bodyString;

		std::vector<std::string>			cgiEnvVector;
		std::vector<char *>					cgiEnv;
		size_t								_max_body_size;


		exec_info_t							exec_info;
		handler_response_t					handler_response;


};

std::ostream &operator << (std::ostream &o, Handler  & pr );


#endif