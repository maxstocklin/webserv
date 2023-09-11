/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/10 23:07:26 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ListeningSocket.hpp"

ListeningSocket::ListeningSocket(std::string serverBlock) : BindingSocket(serverBlock)
{
	backlog = 3; // find an other way
	start_listening();
	test_connection(listening);

}

void ListeningSocket::start_listening()
{
	listening = listen(get_sock(), backlog);
	std::cout << std::endl << GREEN << BOLD << "################################# NEW MASTER SOCKET #################################" << RESET << std::endl;
	std::cout << "socket fd = " << get_sock() << std::endl;
	std::cout << "listening = " << listening << std::endl;
	std::cout << "Endpoint / IP:port comination = " << get_endpoint() << std::endl << std::endl;
	
}

