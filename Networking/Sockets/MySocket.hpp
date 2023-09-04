/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mysocket.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:37 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 14:43:28 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MYSOCKET_HPP
# define MYSOCKET_HPP

# define PORT 8080

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
	public:
		// constructors
		Socket(int domain, int service, int protocol, int port, u_long interface);

		// virtual function to connect to network
		virtual int connect_network(int sock, struct sockaddr_in address) = 0;

		// test sockets and connections
		void test_connection(int item_to_test);

		// getters
		struct sockaddr_in get_address();
		int get_sock();
		int get_connection();

		//setters
		void set_connection(int con);

	private:
		int	connection;
		int	sock_fd;
		struct sockaddr_in address;

		Socket();

};



#endif