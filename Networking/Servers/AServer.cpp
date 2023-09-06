/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:40:57 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/AServer.hpp"

AServer::AServer(int domain, int service, int protocol, int port, u_long interface, int bklog)
{
	socket = new ListeningSocket(domain, service, protocol, port, interface, bklog);
	// delete socket;
	// socket = new ListeningSocket()
}

ListeningSocket * AServer::get_socket()
{
	return (socket);
}
