/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyServer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 15:54:10 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MYSEVER_HPP
# define MYSEVER_HPP

#include "../alibnetworking.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

class Server
{
	public:
		Server(int domain, int service, int protocol, int port, u_long interface, int bklog);
		virtual void launch() = 0;

		// getters
		ListeningSocket * get_socket();
	private:
		Server();
		ListeningSocket * socket;
		virtual void accepter() = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;

};

#endif