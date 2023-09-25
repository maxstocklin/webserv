/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 22:40:20 by max               #+#    #+#             */
/*   Updated: 2023/09/25 19:44:24 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CGI_HANDLER_HPP

# define CGI_HANDLER_HPP

# define CGI_BUFSIZE 65536

// # include "Config.hpp"
#include "Request.hpp"
#include "MasterSocket.hpp"

class MasterSocket;

class CgiHandler
{
	public:
		CgiHandler(Request &request, MasterSocket &config); // sets up env according to the request
		CgiHandler(CgiHandler const &src);
		virtual ~CgiHandler(void);

		CgiHandler   	&operator=(CgiHandler const &src);
		std::string		executeCgi(const std::string &scriptName);	// executes cgi and returns body
	private:
		CgiHandler(void);
		void								_initEnv(Request &request, MasterSocket &config);
		char								**_getEnvAsCstrArray() const;
		int									_getSocket(unsigned int port);
		int									_connectSocket(unsigned int port);
		std::map<std::string, std::string>	_env;
		std::string							_body;
};

// ENV TO IMPLEMENT
// AUTH_TYPE
// CONTENT_LENGTH
// CONTENT_TYPE
// GATEWAY_INTERFACE
// PATH_INFO
// PATH_TRANSLATED
// QUERY_STRING˜

#endif
