#ifndef SimpleSocket_hpp
#define SimpleSocket_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

namespace HDE
{
	class SimpleSocket
	{
		private:
			struct sockaddr_in address;
			int sock;
			int connection;
		public:
			// constructor
			SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
			//virutal function to connect to a network
			virtual int connect_to_network(int sock, struct sockaddr_in address) = 0;
			// function to test sockets and connections
			void test_connection(int);
			// getter functions
			struct sockaddr_in get_address();
			int get_sock();
			int get_connection();
			// setter functions
			void set_connection(int con);
	};
}

#endif /* SimpleSocket_hpp */

