/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectingSocket.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:12:01 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:43:41 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ConnectingSocket.hpp"

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : ASocket(domain, service, protocol, port, interface)
{
	//establish network connection
	set_connection(connect_network(get_sock(), get_address()));
	test_connection(get_connection());

}
int ConnectingSocket::connect_network(int sock, struct sockaddr_in address)
{
    return (connect(sock, (struct sockaddr *)&address, sizeof(address)));
}

