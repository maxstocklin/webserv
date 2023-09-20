/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/20 00:47:01 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/WebServer.hpp"
#include "../../Includes/FetchHtmlBody.hpp"
#include "../../Includes/Handler.hpp"

WebServer::WebServer(char *config_file, char **env) : AServer(config_file), env(env)
{
	memset(buffer, 0, sizeof(buffer));
	launch();
}

WebServer::~WebServer()
{
	
}

void WebServer::accepter(ListeningSocket *master_socket)
{
	struct sockaddr_in address = master_socket->get_address();
	int addrlen = sizeof(address);

	if ((new_socket = accept(master_socket->get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		std::cout << "PROBS 1\n";
		if (errno == ENOENT)
			handler.set_response_status_code(404);  // Not Found
		else if (errno == EACCES)
			handler.set_response_status_code(403);  // Forbidden
		else
			handler.set_response_status_code(500);  // Internal Server Error
		handler.set_response_htmlContentType("text/html");
		return;
	}

	struct timeval timeout;
	timeout.tv_sec = 5;  // 5 seconds timeout
	timeout.tv_usec = 0;

	// Set receive timeout
	if (setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		std::cout << "WOOP WOOP1\n";
		handler.set_response_status_code(408);  // Timeout
		handler.set_response_keepAlive(false);
		handler.set_response_htmlContentType("text/html");
		close(new_socket);
	}

	// Set send timeout
	else if (setsockopt(new_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		std::cout << "WOOP WOOP2\n";
		handler.set_response_status_code(408);  // Timeout
		handler.set_response_keepAlive(false);
		handler.set_response_htmlContentType("text/html");
		close(new_socket);
	}

	memset(buffer, 0, sizeof(buffer));
	std::cout << "PROBS 1\n";

	completeData.clear();
	while (true)
	{
		ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);


		if (bytes_read > 0)
		{
			completeData.append(buffer, bytes_read);
			if (completeData.find("\r\n\r\n") != std::string::npos)
				break;

		}
		else if (bytes_read == 0)
		{
			break;
		}
		else
		{
			handler.set_response_status_code(408);  // Request Timeout
			handler.set_response_htmlContentType("text/html");
			return;
		}
	}
}

void WebServer::handle(ListeningSocket *master_socket)
{
	std::cout << "###################### HTTP REQUEST ######################" << std::endl  << std::endl;
	std::cout << buffer << std::endl;

	handler.setBuffer(completeData);
	handler.parseRequest(master_socket, env);
	// std::cout << handler << std::endl;

	handler.makeFullLocalPath(master_socket);
	handler.getPathResponse(master_socket, new_socket);
}

void WebServer::responder(ListeningSocket *master_socket)
{
	Responder resp(handler, master_socket->get_error_pages(), new_socket);
}


void	WebServer::addToSet(const int i, fd_set &set)
{
	FD_SET(i, &set);
	if (i > _biggest_fd)
		_biggest_fd = i;
}

void	ServerManager::removeFromSet(const int i, fd_set &set)
{
	FD_CLR(i, &set);
	if (i == _biggest_fd)
		_biggest_fd--;
}

/* initialize recv+write fd_sets and add all server listening sockets to _recv_fd_pool. */
void	WebServer::initializeSets()
{
	FD_ZERO(&_recv_fd_pool);
	FD_ZERO(&_write_fd_pool);


	// adds servers sockets to _recv_fd_pool set
	for(std::vector<ServerConfig>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		addToSet(it->get_sock(), _recv_fd_pool);
		_servers_map.insert(std::make_pair(it->get_sock(), *it));
	}
	// at this stage _biggest_fd will belong to the last server created.
	// TODO: Check if it's the case for us too
	_biggest_fd = _servers.back().get_sock();
}










































void	WebServer::launch()
{
	fd_set  recv_set_cpy;
	fd_set  write_set_cpy;
	int select_activity;
	int		count = 0;

	_biggest_fd = 0;
	initializeSets();

	struct timeval timer;
	while (1)
	{
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		recv_set_cpy = _recv_fd_pool;
		write_set_cpy = _write_fd_pool;
		std::cout << "============= WAITING FOR NEXT CONNECT =============" << std::endl;

		if ((select_activity = select(_biggest_fd + 1, &recv_set_cpy, &write_set_cpy, NULL, &timer)) < 0 )
		{
			if ((errno!=EINTR))
			{
				std::cerr << "select error && select_activity = " << select_activity << std::endl;
				perror("select");
			}
			// TODO: remove to avoid crash
			// continue; ? or exit(1);
			throw std::runtime_error("Error: select error " + strerror(errno));
		}

		for (int i = 0; i <= _biggest_fd; ++i)
		{
			if (FD_ISSET(i, &recv_set_cpy) && _servers_map.count(i))
				acceptNewConnection(_servers_map.find(i)->second);
			else if (FD_ISSET(i, &recv_set_cpy) && _clients_map.count(i))
				readRequest(i, _clients_map[i]);
			else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
			{
				int cgi_state = _clients_map[i].response.getCgiState(); // 0->NoCGI 1->CGI write/read to/from script 2-CGI read/write done
				if (cgi_state == 1 && FD_ISSET(_clients_map[i].response._cgi_obj.pipe_in[1], &write_set_cpy))
					sendCgiBody(_clients_map[i], _clients_map[i].response._cgi_obj);
				else if (cgi_state == 1 && FD_ISSET(_clients_map[i].response._cgi_obj.pipe_out[0], &recv_set_cpy))
					readCgiResponse(_clients_map[i], _clients_map[i].response._cgi_obj);
				else if ((cgi_state == 0 || cgi_state == 2)  && FD_ISSET(i, &write_set_cpy))
					sendResponse(i, _clients_map[i]);
			}
		}
		checkTimeout();


		// Check select_activity on each master socket
		for (size_t i = 0; i < get_socket_vector().size(); i++)
		{
			int master_socket_fd = get_socket(i)->get_sock();

			if (FD_ISSET(master_socket_fd, &readfds))
			{
				handler.set_response_status_code(0);
				accepter(get_socket(i));
				if (handler.get_handler_response().statusCode == 0)
					handle(get_socket(i));
				responder(get_socket(i));
				std::cout << "count = " << count << std::endl;
				std::cout << "============= DONE =============" << std::endl;
				count++;

				// // if (handler.get_connection() == "keep-alive")
				// {
				// 	//add new socket to array of sockets
				// 	for (int j = 0; j < MAX_CLIENTS; j++)
				// 	{
				// 		//if position is empty
				// 		if( client_socket[j] == 0 )
				// 		{
				// 			close(new_socket);
				// 			client_socket[j] = new_socket;
				// 			std::cout << "Adding to list of sockets as " << j << std::endl;
				// 			break;
				// 		}
				// 	}
				// }
				// // else
				// {
				// }
			}
		}
		for (int j = 0; j < MAX_CLIENTS; j++)
		{
			sd = client_socket[j];

			if (FD_ISSET(sd, &readfds))
			{
				memset(buffer, 0, sizeof(buffer));
				if ((valread = read( sd , buffer, sizeof(buffer))) == 0)
				{
					close(sd);
					client_socket[j] = 0;
				}
				else
				{
					close(new_socket);
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}
}





/**
 * - Reads data from client and feed it to the parser.
 * Once parser is done or an error was found in the request,
 * socket will be moved from _recv_fd_pool to _write_fd_pool
 * and response will be sent on the next iteration of select().
 */
void	WebServer::readRequest(const int &i, Client &c)
{
	int		bytes_read = 0;
	char	buffer[40000];

	// c.completeData.clear(); // TODO: place it elsewhere, at the end of the response sent

	bytes_read = read(i, buffer, sizeof(buffer) - 1);

	if (bytes_read == 0)
	{
		std::cout << YELLOW << BOLD << "webserv: Client " << i " Closed Connection" << RESET << std::endl;
		closeConnection(i);
		return ;
	}
	else if (bytes_read < 0)
	{
		std::cout << RED << BOLD << "webserv: Client " << i " read error " << strerror(errno) << RESET << std::endl;
		closeConnection(i);
		handler.set_response_status_code(408);  // Request Timeout
		handler.set_response_htmlContentType("text/html");
		return; // maybe not return
	}
	else if (bytes_read != 0)
	{
		c.updateTime();
		c.completeData.append(buffer, bytes_read);
		memset(buffer, 0, sizeof(buffer));
	}

	if (c.request.get_handler_response().statusCode != 0 || requestCompletelyReceived(c.completeData) == true)
	{
		if (requestCompletelyReceived(c.completeData) == true)
		{
			c.request.setBuffer(c.completeData); // todo: only if it's not an error status code 
			c.request.parseRequest(c.server, env);
			// std::cout << c.request << std::endl;

			// c.request.makeFullLocalPath(server);
			// c.request.getPathResponse(server, new_socket);

		}

		std::cout << CYAN << "Request Recived From Socket " << i << " Method= " << c.request.get_method() << " URI= " << c.request.get_path() << RESET << std::endl;
		assignServer(c);

		c.buildResponse();


		if (c.response.getCgiState())
		{
			handleReqBody(c);
			addToSet(c.response._cgi_obj.pipe_in[1],  _write_fd_pool);
			addToSet(c.response._cgi_obj.pipe_out[0],  _recv_fd_pool);
		}
		removeFromSet(i, _recv_fd_pool);
		addToSet(i, _write_fd_pool);
	}
}



/* Assign server_block configuration to a client based on Host Header in request and server_name*/
void    ServerManager::assignServer(Client &c)
{
	c.ser
	for (std::vector<ServerConfig>::iterator it = sockets.begin();
		it != sockets.end(); ++it)
	{
		c.ser
		if (c.server.get_host() == it->get_host() &&
			c.server.get_port() == it->get_port() &&
			c.request.get_server_name() == it->get_server_name())
		{
			c.setServer(*it);
			return ;
		}
	}
}


// remove all the tabs and spaces at the extremeties of the string
// TODO: this is a utils function from ServerConfig class
std::string WebServer::trimWhiteSpaces(const std::string &str)
{
	std::size_t first = str.find_first_not_of(" \t");

	if (std::string::npos == first) // if the string is only made of white spaces, return the string
		return (str);

	std::size_t last = str.find_last_not_of(" \t");

	return (str.substr(first, (last - first + 1)));
}


bool WebServer::requestCompletelyReceived(std::string completeData)
{
	size_t		startPos;
	size_t		endPos

	size_t headersEndPos = completeData.find("\r\n\r\n");
	if (headersEndPos == std::string::npos)
		return false;  // Headers aren't fully received yet.

	std::string headers = completeData.substr(0, headersEndPos);
	std::string body = completeData.substr(headersEndPos + 4);  // +4 to skip "\r\n\r\n"

	// lower case the headers to find "content-length" and "Content-Length"
	std::string lowerCaseHeaders = toLowerCase(headers);

	size_t contentLengthPos = lowerCaseHeaders.find("\r\ncontent-length:");

	if (contentLengthPos != std::string::npos)
	{
		startPos = contentLengthPos + 17; // +15 to skip "\r\ncontent-length:"
		endPos = lowerCaseHeaders.find("\r\n", startPos);

		if (endPos != std::string::npos)
		{
			std::string lengthStr = trimWhiteSpaces(headers.substr(startPos, endPos - startPos));

			try
			{
				int contentLength = std::stoi(lengthStr);
				if (body.size() == static_cast<size_t>(contentLength))
					return true;
				else if (body.size() > static_cast<size_t>(contentLength))
				{
					// todo: set error code, body size is too big
				}
			}
			catch (const std::exception &e)
			{
				// TODO: ERROR CODE?
				std::cerr << e.what() << '\n';
				throw std::runtime_error("Error with content-length --> not an int" + bodyLength);
				return false;
			}
		}
	}

	if (lowerCaseHeaders.find("transfer-encoding: chunked") != std::string::npos)
	{
		if (body.find("0\r\n\r\n") != std::string::npos)
			return true;
	}

	// No body, just headers
	if (contentLengthPos == std::string::npos && lowerCaseHeaders.find("transfer-encoding:") == std::string::npos)
		return true;

	// No body yet
	return false;
}


/* Closes connection from fd i and remove associated client object from _clients_map */
void	WebServer::closeConnection(const int i)
{
	if (FD_ISSET(i, &_write_fd_pool))
		removeFromSet(i, _write_fd_pool);
	if (FD_ISSET(i, &_recv_fd_pool))
		removeFromSet(i, _recv_fd_pool);
	close(i);
	_clients_map.erase(i);
}





/**
 * Accept new incomming connection.
 * Create new Client object and add it to _client_map
 * Add client socket to _recv_fd_pool
*/
void	WebServer::acceptNewConnection(ListeningSocket &serv)
{
	struct sockaddr_in	addrlen;
	long				addrlen = sizeof(addrlen);
	int					client_sock; // TODO: new_socket
	Client				new_client(serv);

	if ((client_sock = accept(serv.get_sock(), (struct sockaddr *)&addrlen, (socklen_t*)&addrlen)) == -1)
	{
		if (errno == ENOENT)
			new_client.request.set_response_status_code(404);  // Not Found
		else if (errno == EACCES)
			new_client.request.set_response_status_code(403);  // Forbidden
		else
			new_client.request.set_response_status_code(500);  // Internal Server Error
		new_client.request.set_response_htmlContentType("text/html");
		return;
	}
	std::cout << BLUE << BOLD << "New Connection, Assigned Socket " << client_sock << RESET << std::endl;

	addToSet(client_sock, _recv_fd_pool);

	if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << RED << BOLD << "webserv: fcntl error " << strerror(errno) <<  RESET << std::endl;
		removeFromSet(client_sock, _recv_fd_pool);
		close(client_sock);
		return ;
	}
	new_client.setSocket(client_sock);
	if (_clients_map.count(client_sock) != 0)
		_clients_map.erase(client_sock);
	_clients_map.insert(std::make_pair(client_sock, new_client));
}
