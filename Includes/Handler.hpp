/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/18 15:41:25 by srapopor         ###   ########.fr       */
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

		// SETTERS
		void						set_response_status_code(int code);
		void						set_response_keepAlive(bool res);
		void						set_response_htmlContentType(std::string type);
		void						set_response_htmlBody(std::string body);

		// GETTERS
		char						*get_buffer();
		std::string					get_completeData();

		std::string					get_method();
		std::string					get_path();
		std::string					get_fullLocalPath();
		std::string					get_base_index();
		std::string					get_query_string();
		std::string					get_filename();
		std::string					get_connection();
		int							get_contentLength();
		std::vector<std::string>	get_cgiEnvVector();
		std::vector<char *>			get_cgiEnv();

		exec_info_t					get_exec_info();
		handler_response_t			get_handler_response();
		void 						setBody(std::string _completeData);

	private:
		char						_buffer[300000];
		std::string					_completeData;
		std::string					body;

		std::string					method;
		std::string					path;
		std::string					fullLocalPath;
		std::string					base_index;
		std::string					query_string;
		std::string					filename;
		std::string					connection;
		int							contentLength;
		std::vector<std::string>	cgiEnvVector;
		std::vector<char *>			cgiEnv;

		exec_info_t					exec_info;
		handler_response_t			handler_response;


};

std::ostream &operator << (std::ostream &o, Handler  & pr );


#endif