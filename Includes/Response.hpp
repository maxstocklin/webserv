/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:29:46 by max               #+#    #+#             */
/*   Updated: 2023/09/25 00:27:01 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "ResponseHeader.hpp"
# include "Request.hpp"
# include "MasterSocket.hpp"

# include "CgiHandler.hpp"
# include "AutoIndexGenerator.hpp"



#include "Colors.hpp"

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
#include <map>
#include <vector>
#include <list>

#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

class MasterSocket;

typedef struct	s_listen {
	unsigned int	host;
	int			port;
}				t_listen;

class Response {
public:
	Response(void);
	Response(const Response & src);
	~Response(void);

	Response & operator=(const Response & src);

	// Getter functions
	std::string		getResponse(void);

	// Member functions
	void			call(Request & request, MasterSocket &requestConf);

	void			getMethod(Request & request, MasterSocket &requestConf);
	void			postMethod(Request & request, MasterSocket &requestConf);
	void			deleteMethod(Request & request, MasterSocket &requestConf);

	int				readContent(void);
	int				writeContent(std::string content);
	int				fileExists(std::string path);
	std::string		readHtml(const std::string& path);


	void			makeFullLocalPath(MasterSocket *master_socket, const std::string &path, const std::string &method, Location &target_location);

private:
	std::string					_response;
	std::string					_path;
	int							_code;
	std::string					_type;
	bool						_isAutoIndex;
	t_listen					_hostPort;
	std::map<int, std::string>	_errorMap;


	// mine
	std::string							fullLocalPath;
	std::string							base_index;

	static std::map<std::string, void (Response::*)(Request &, MasterSocket &)>	_method;
	static std::map<std::string, void (Response::*)(Request &, MasterSocket &)>	initMethods();
};

#endif
