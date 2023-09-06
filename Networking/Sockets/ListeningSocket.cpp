/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfinocie <vfinocie@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/06 13:57:22 by vfinocie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklog) : BindingSocket(domain, service, protocol, port, interface)
{
	backlog = bklog;
	start_listening();
	test_connection(listening);
}

void ListeningSocket::start_listening()
{
	listening = listen(get_sock(), backlog);
	std::cout << "sock = " << get_sock() << std::endl;
	std::cout << "listening = " << listening << std::endl;
}