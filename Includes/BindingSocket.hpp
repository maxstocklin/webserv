/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BindingSocket.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/10 23:07:03 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BINDINGSOCKET_HPP
# define BINDINGSOCKET_HPP

#include "Colors.hpp"

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ASocket.hpp"

class BindingSocket : public ASocket
{
	public:
		// constructors
		BindingSocket(std::string serverBlock);

		// virtual function to connect to network
		int connect_network(int Sock_fd, struct sockaddr_in address);  // todo --> sock??

	private:
		BindingSocket();

};



#endif