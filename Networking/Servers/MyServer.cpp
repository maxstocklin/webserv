/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyServer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 13:16:14 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MyServer.hpp"

Server::Server(int domain, int service, int protocol, int port, u_long interface, int bklog)
{
	socket = new ListeningSocket(domain, service, protocol, port, interface, bklog);
	// delete socket;
	// socket = new ListeningSocket()
}

ListeningSocket * Server::get_socket()
{
	return (socket);
}
