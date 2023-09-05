/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BindingSocket.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:43:57 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BINDINGSOCKET_HPP
# define BINDINGSOCKET_HPP

# define PORT 8080

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ASocket.hpp"

class BindingSocket : public ASocket
{
	public:
		// constructors
		BindingSocket(int domain, int service, int protocol, int port, u_long interface);

		// virtual function to connect to network
		int connect_network(int Sock_fd, struct sockaddr_in address);  // todo --> sock??

	private:
		BindingSocket();

};



#endif