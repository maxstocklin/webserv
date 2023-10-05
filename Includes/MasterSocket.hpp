	/* ************************************************************************** */
	/*                                                                            */
	/*                                                        :::      ::::::::   */
	/*   MasterSocket.hpp                                   :+:      :+:    :+:   */
	/*                                                    +:+ +:+         +:+     */
	/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
	/*                                                +#+#+#+#+#+   +#+           */
	/*   Created: 2023/09/04 14:53:09 by mstockli          #+#    #+#             */
	/*   Updated: 2023/09/24 23:40:08 by max              ###   ########.fr       */
	/*                                                                            */
	/* ************************************************************************** */

	#ifndef MASTERGSOCKET_HPP
	# define MASTERGSOCKET_HPP

	# define PRINT 0 		// set to 1 to print request and response

	#include "Colors.hpp"
	#include "BindingSocket.hpp"
	#include "Request.hpp"
	#include "Response.hpp"
	#include "Utils.hpp"

	#include <fcntl.h>

	class Response;

	// MasterSocket represents each server
	// it is first created to parse the config file, create, bind, and listen to each master socket
	// it holds eversy attributes as well, making it easier to access the data later
	// once a request is received, it is parsed here to create the response
	class MasterSocket : public BindingSocket
	{
		public:
			MasterSocket(std::string serverBlock);

			// parsing config
			void								start_listening();
			
			// parse HTTP request
			void								handle(long socket, char **env);	// main parsing function
			void								parseChunks(long socket);

			const std::string					&get_path() const;
			void								set_path(std::string path);
			std::map<long, std::string>			_requests;					// at first, this map is the request, after the parsing, it's the response
			std::map<long, bool>				_keepAlive;

		private:
			int							backlog;					// hardcoded
			int							listening;
			std::string					fullLocalPath;


			MasterSocket();
	};
	#endif