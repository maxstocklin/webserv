#ifndef ListeningSocket_hpp
#define ListeningSocket_hpp

#include <stdio.h>

#include "BindingSocket.hpp"

namespace HDE
{
	class ListeningSocket: public BindingSocket
	{
		private:
			//waiting line size; if =10 the 11th is canceled.
			int backlog;
			int listening;
		public:
			//constructor
			ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklg);
			void start_listening();

			int get_listening();
			int get_backlog();
	};
}

#endif /* ListeningSocket_hpp */