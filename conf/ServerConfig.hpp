/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:47:04 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/08 14:12:09 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <stdio.h>
#include <iostream>
#include <vector>

class ServerConfig
{
	public:
		ServerConfig(char **server);
		~ServerConfig();


	private:
		int							port;
		std::string					host;
		std::string					server_name;
		std::string					index;
		std::string					error_page;
		int							client_max_body_size;
		std::vector<std::string>	allow_methods;
		std::string					root;


		int							service;
		int							protocol;
		u_long						interface;
		int							bklog;

};

#endif
