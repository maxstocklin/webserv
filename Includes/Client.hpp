/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/18 19:42:29 by max               #+#    #+#             */
/*   Updated: 2023/09/20 14:28:01 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "WebServer.hpp"
#include "Handler.hpp"
#include "Responder.hpp"
#include "ListeningSocket.hpp"

/**
		 Client
	 - Stores all information related to the client such as socket and address
		 along with request and response objects. each Client also have an object of the server it's conntected to.
**/
class Client
{
		public:
				Client();
				Client(ListeningSocket &server);
				~Client();

				const int										&getSocket() const;
				const struct sockaddr_in						&getAddress() const;
				const HttpRequest								&getRequest() const;
				const time_t									&getLastTime() const;

				void											setSocket(int &);
				void											setServer(ListeningSocket &server);
				void											buildResponse();
				void											updateTime();

				void											clearClient();

				void											setServer(ListeningSocket &server);


				Responder									response;
				Handler										request;
				ListeningSocket								server;

				std::string									completeData;	// TODO: add to private

		private:
				int											_client_socket;
				struct sockaddr_in							_client_address;
				time_t										_last_msg_time;

};


#endif // CLIENT_HPP