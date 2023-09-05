/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MySocket.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:21 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 11:11:59 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MySocket.hpp"

Socket::Socket(int domain, int service, int protocol, int port, u_long interface)
{
	// define address structure
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);

	//establish socket
	sock_fd = socket(domain, service, protocol);

	// //establish network connection
	// connection = connect_network(sock_fd, address);
	// test_connection(connection);
}

// test connection virtual function
void Socket::test_connection(int item_to_test)
{
	// confirm the socket or connection has been properly established
	if (item_to_test < 0)
	{
		perror("ERROR: Failed to connect...");
		exit(EXIT_FAILURE);
	}
}

void Socket::set_connection(int con)
{
	connection = con;
}

struct sockaddr_in Socket::get_address()
{
	return address;
}

int Socket::get_sock()
{
	return sock_fd;
}

int Socket::get_connection()
{
	return connection;
}

