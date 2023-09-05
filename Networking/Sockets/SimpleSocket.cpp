#include "SimpleSocket.hpp"

// default constructor

// interface = my ip address;
HDE::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long interface)
{
	// define address structure
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	// connect the socket to the network
	// establish socket
	sock = socket(domain, service, protocol);
	test_connection(sock);
}

// test connection virtual function
// test the connection once it is set up.
void HDE::SimpleSocket::test_connection(int item_to_test)
{
	// confirm that the socket or connection has been properly established
	if (item_to_test < 0)
	{
		perror("Failed to connect...");
		exit(EXIT_FAILURE);
	}
}

// getter functions
struct sockaddr_in HDE::SimpleSocket::get_address()
{
	return address;
}

int HDE::SimpleSocket::get_sock()
{
	return sock;
}

// int HDE::SimpleSocket::get_connection()
// {
// 	return connection;
// }

// // setter functions
// void HDE::SimpleSocket::set_connection(int con)
// {
// 	connection = con;
// }