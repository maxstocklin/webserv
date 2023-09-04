/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 18:09:22 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TestServer.hpp"

TestServer::TestServer() : Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
	launch();
}

void TestServer::accepter()
{
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof(address);
	new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	read(new_socket, buffer, 30000);
}

void TestServer::handler()
{
	std::cout << buffer << std::endl;
}

void TestServer::responder()
{
	char *hello = "hello my name is jeff\n";
	write(new_socket, hello, strlen(hello));
	close(new_socket);
}

void TestServer::launch()
{
	int count = 0;
	while (1)
	{
		std::cout << "============= WAITING =============" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "============= DONE =============" << std::endl;
		std::cout << "count = " << count << std::endl;
		count++;
	}
}

