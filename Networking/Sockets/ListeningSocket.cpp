/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 15:26:40 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"

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