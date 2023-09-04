/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectingSocket.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 14:59:11 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTINGSOCKET_HPP
# define CONNECTINGSOCKET_HPP

# define PORT 8080

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include "MySocket.hpp"

class ConnectingSocket : public Socket
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