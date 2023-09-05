/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:42:38 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASEVER_HPP
# define ASEVER_HPP

#include "ListeningSocket.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

class AServer
{
	public:
		AServer(int domain, int service, int protocol, int port, u_long interface, int bklog);
		virtual void launch() = 0;

		// getters
		ListeningSocket * get_socket();
	private:
		AServer();
		ListeningSocket * socket;
		virtual void accepter() = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;

};

#endif