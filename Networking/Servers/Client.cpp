/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/18 19:44:28 by max               #+#    #+#             */
/*   Updated: 2023/09/19 21:39:53 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Client.hpp"

Client::Client()
{
	_last_msg_time = time(NULL);
}

Client::~Client() {}

Client::Client(ListeningSocket &server)
{
	setServer(server);

	request.setMaxBodySize(server.get_client_max_body_size());
	_last_msg_time = time(NULL);
}

void	Client::setSocket(int &sock)
{
	_client_socket = sock;
}

void	Client::setServer(ListeningSocket &server)
{
	response.setServer(server);
}


const int	&Client::getSocket() const
{
	return (_client_socket);
}

const HttpRequest	&Client::getRequest() const
{
	return (request);
}

const struct sockaddr_in	&Client::getAddress() const
{
	return (_client_address);
}

const time_t	&Client::getLastTime() const
{
	return (_last_msg_time);
}


void	Client::buildResponse()
{
	response.setRequest(this->request);
	response.buildResponse(); // todo write function
}

void	Client::updateTime()
{
	_last_msg_time = time(NULL);
}

void	Client::clearClient()
{
	response.clear();
	request.clear();
}