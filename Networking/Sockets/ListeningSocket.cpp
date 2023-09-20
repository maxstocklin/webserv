/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/18 18:58:51 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ListeningSocket.hpp"

ListeningSocket::ListeningSocket(std::string serverBlock) : BindingSocket(serverBlock)
{
	backlog = 3; // find an other way
	start_listening();
	test_connection(listening);

	// ensure that the master_socket socket operates in non-blocking mode
	test_connection(fcntl(get_sock(), F_SETFL, O_NONBLOCK)); // possible to add FD_CLOEXEC arg too, which closes automatically the fd when execve is called
}

void ListeningSocket::start_listening()
{
	listening = listen(get_sock(), backlog);
	struct timeval timeout;
	timeout.tv_sec = 5;  // 5 seconds timeout
	timeout.tv_usec = 0;

	// Set receive timeout
	if (setsockopt(get_sock(), SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throw std::runtime_error("Error: setsockopt() timeout");

	// Set send timeout
	if (setsockopt(get_sock(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throw std::runtime_error("Error: setsockopt() timeout");

	std::cout << std::endl << GREEN << BOLD << "################################# NEW MASTER SOCKET #################################" << RESET << std::endl;
	std::cout << "socket fd = " << get_sock() << std::endl;
	std::cout << "listening = " << listening << std::endl;
	std::cout << "Endpoint / IP:port comination = " << get_endpoint() << std::endl << std::endl;
	
}

