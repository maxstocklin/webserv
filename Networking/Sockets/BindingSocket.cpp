#include "BindingSocket.hpp"

//constructor
HDE::BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface)
{
	//set_connection(connect_to_network(get_sock(), get_address()));
	connect_to_network(get_sock(), get_address());
	test_connection(binding);
}

//mutators
// definition of connect_toNetwork virtual function
void HDE::BindingSocket::connect_to_network(int sock, struct sockaddr_in address)
{
	//return bind(sock, (struct sockaddr_in *)&address. sizeof(address));
	binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
}

//getter
int HDE::BindingSocket::get_binding()
{
	return binding;
}