#ifndef Binding_Socket_hpp
#define Binding_Socket_hpp

#include <stdio.h>
#include "SimpleSocket.hpp"

namespace HDE
{
	class BindingSocket: public SimpleSocket
	{
		private:
			int binding;
			void connect_to_network(int sock, struct sockaddr_in address);
		public:
			//constructor
			BindingSocket(int domain, int service, int protocol, int port, u_long interface);
			//virtual function from parent

			int get_binding();
	};
}

#endif /* BindingSocket_hpp */