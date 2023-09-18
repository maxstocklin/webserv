/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BindingSocket.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:12:01 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/18 12:33:45 by srapopor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/BindingSocket.hpp"

BindingSocket::BindingSocket(std::string serverBlock) : ASocket(serverBlock)
{
	//establish network connection
	set_connection(connect_network(get_sock(), get_address()));
	test_connection("Binding Socket", get_connection());

}

int BindingSocket::connect_network(int Sock_fd, struct sockaddr_in address)
{
	return (bind(Sock_fd, (struct sockaddr *)&address, sizeof(address)));
}
