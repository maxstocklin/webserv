/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:55:21 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:37:32 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/ASocket.hpp"

ASocket::ASocket(std::string serverBlock)
{
	config = new ServerConfig(serverBlock);
	
	// TODO: CREATE A NEW FUNCTION FOR SET UP
	// define address structure
	address.sin_family = get_domain();
	address.sin_port = htons(get_port());
	address.sin_addr.s_addr = htonl(get_interface());

	// change interface based on the host:
	// if (host == "0.0.0.0")
	// 	address.sin_addr.s_addr = htonl(INADDR_ANY);
	// else
	// {
	// 	if (inet_pton(AF_INET, host.c_str(), &(address.sin_addr)) <= 0)
	// 	{
	// 		// Handle error here
	// 	}
	// }

	//establish socket
	sock_fd = socket(get_domain(), get_service(), get_protocol());
}

ASocket::~ASocket()
{

}

// test connection virtual function
void ASocket::test_connection(int item_to_test)
{
	// confirm the socket or connection has been properly established
	if (item_to_test < 0)
		throw std::runtime_error("ERROR: Failed to connect...");
	// {
	// 	perror("ERROR: Failed to connect...");
	// 	exit(EXIT_FAILURE);
	// }
}

// --------------- GETTERS & SETTERS ---------------

void ASocket::set_connection(int con)
{
	connection = con;
}

struct sockaddr_in ASocket::get_address()
{
	return (address);
}

int ASocket::get_sock()
{
	return (sock_fd);
}

int ASocket::get_connection()
{
	return (connection);
}

ServerConfig *ASocket::get_config()
{
	return (this->config);
}

int ASocket::get_port()
{
	return (get_config()->get_port());
}

int ASocket::get_domain()
{
	return (get_config()->get_domain());
}

std::string ASocket::get_host()
{
	return (get_config()->get_host());
}

std::string ASocket::get_server_name()
{
	return (get_config()->get_server_name());
}

std::string ASocket::get_index()
{
	return (get_config()->get_index());
}

std::map<int, std::string> ASocket::get_error_pages()
{
	return (get_config()->get_error_pages());
}

int ASocket::get_client_max_body_size()
{
	return (get_config()->get_client_max_body_size());
}

Location ASocket::get_rootLocation()
{
	return (get_config()->get_rootLocation());
}

std::vector<Location> ASocket::get_locations()
{
	return (get_config()->get_locations());
}

int ASocket::get_service()
{
	return (get_config()->get_service());
}

u_long ASocket::get_interface()
{
	return (get_config()->get_interface());
}

int ASocket::get_protocol()
{
	return (get_config()->get_protocol());
}

std::string ASocket::get_endpoint()
{
	std::string res = get_config()->get_host() + ':' + std::to_string(get_config()->get_port());

	return (res);
}
