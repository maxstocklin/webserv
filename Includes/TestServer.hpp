/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/05 21:39:31 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTSEVER_HPP
# define TESTSEVER_HPP

#include "AServer.hpp"

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

class TestServer : public AServer
{
	public:
		TestServer();
		void launch();

	private:
		char buffer[30000] = {0};
		int new_socket;
		bool isFaviconRequest = false; // Add this as a member variable in your TestServer class
		void accepter();
		void handler();
		void responder();

};

#endif