/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BindingSocket.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:12:01 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:43:37 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : ASocket(domain, service, protocol, port, interface)
{
	//establish network connection
	set_connection(connect_network(get_sock(), get_address()));
	test_connection(get_connection());

}
int BindingSocket::connect_network(int Sock_fd, struct sockaddr_in address)
{
	return (bind(Sock_fd, (struct sockaddr *)&address, sizeof(address)));
}
