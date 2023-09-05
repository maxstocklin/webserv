/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectingSocket.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:44:23 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTINGSOCKET_HPP
# define CONNECTINGSOCKET_HPP

# define PORT 8080

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ASocket.hpp"

class ConnectingSocket : public ASocket
{
	public:
		// constructors
		ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);

		// virtual function to connect to network
		int connect_network(int sock, struct sockaddr_in address);

	private:
		ConnectingSocket();

};



#endif