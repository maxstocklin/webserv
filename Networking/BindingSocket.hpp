#ifndef Binding_Socket_hpp
#define Binding_Socket_hpp

#include <stdio.h>
#include "SimpleSocket.hpp"

namespace HDE
{
	class BindingSocket: public SimpleSocket
	{
		public:
			//constructor
			BindingSocket(int domain, int service, int protocol, int port, u_long interface);
			//virtual function from parent
			int connect_to_network(int sock, struct sockaddr_in address);

	};
}

#endif /* BindingSocket_hpp */