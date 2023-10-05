/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/10/06 00:42:01 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/WebServer.hpp"

WebServer::WebServer(char *config_file, char **env) : AServer(config_file), env(env)
{
	launch();
}

WebServer::~WebServer()
{
	
}

void	WebServer::initializeSets()
{
	FD_ZERO(&_fd_set);
	_max_fd = 0;
	
	// adds servers sockets to _fd_set set
	for(std::vector<MasterSocket>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		FD_SET(it->get_sock(), &_fd_set);
		_servers.insert(std::make_pair(it->get_sock(), *it));
		if (it->get_sock() > _max_fd)
			_max_fd = it->get_sock();
		std::cout << "Setting up " << it->get_host() << ":" << it->get_port() << "..." << std::endl;
	}
	if (_max_fd == 0)
	{
		throw std::runtime_error("Could not setup server!");
	}
}

int	WebServer::readRequest(long socket, MasterSocket &serv)
{
	int		bytes_read = 0;
	char	buffer[40000];

	memset(buffer, 0, sizeof(buffer));

	bytes_read = read(socket, buffer, sizeof(buffer) - 1);

	if (bytes_read == 0 || bytes_read == -1)
	{
		if (socket > 0)
		{
			close(socket);
		}
		serv._requests.erase(socket);

		if (!bytes_read)
			std::cout << YELLOW << BOLD << "Client " << socket << " closed connection" << RESET << std::endl;
		else
			std::cerr << RED << BOLD << "ERROR: Client " << socket << " read returned -1." << RESET << std::endl;
		return (-1);
	}

	else if (bytes_read != 0)
	{
		serv._requests[socket].append(buffer, bytes_read);
	}
	
	if (requestCompletelyReceived(serv._requests[socket], serv, socket) == true)
		return 0;
	else
		return 1;
}

long	WebServer::writeRequest(long socket, MasterSocket &serv)
{
	static std::map<long, size_t>	sent;

	if (sent.find(socket) == sent.end()) // check if something was already sent
		sent[socket] = 0;

	std::string	str = serv._requests[socket].substr(sent[socket], 30000); // create a string from what was already sent (if any --> sent[socket] != 0) until either the end of the response of max 30000 size
	int	ret = write(socket, str.c_str(), str.size()); // write the string
	if (ret == -1) // if an error occured with write()
	{
		if (socket > 0)
		{
			close(socket);
		}

		serv._requests.erase(socket);
		sent[socket] = 0;
		return (-1);
	}
	else
	{
		sent[socket] += ret; // save how much of the response was already witten()
		if (sent[socket] >= serv._requests[socket].size()) // if all of the response was written()
		{
			serv._requests.erase(socket); // erase the response
			sent[socket] = 0; // reset the sent size

			if (serv._keepAlive.find(socket) != serv._keepAlive.end() && !serv._keepAlive[socket])
			{
				close(socket);
			}
			return (0);
		}
		else	// if part of the response was written
			return (1);
	}
}

void	WebServer::launch()
{
	initializeSets();

	signal(SIGPIPE, SIG_IGN);

	while (42)
	{
		fd_set			reading_set;
		fd_set			writing_set;
		int				select_activity = 0;
		struct timeval	timeout;

		while (select_activity == 0)
		{
			timeout.tv_sec  = 3;
			timeout.tv_usec = 0;
			ft_memcpy(&reading_set, &_fd_set, sizeof(_fd_set));
			FD_ZERO(&writing_set);		// reset writing set
			for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)	// add every new socket that is ready to the writing set
				FD_SET(*it, &writing_set);

			long maxtmp = 0;
			for (int i = 0; i <= _max_fd; i++)
			{
				if (FD_ISSET(i, &_fd_set))
				{
					if (i > maxtmp)
						maxtmp = i;
				}
				else if (i > 10)
				{
					close(i);
					_sockets.erase(i);
				}
			}
			_max_fd = maxtmp;
			select_activity = select(_max_fd + 1, &reading_set, &writing_set, NULL, &timeout); //
		}
		
		if (select_activity > 0)
		{
			// check write
			for (std::vector<int>::iterator it = _ready.begin() ; select_activity && it != _ready.end() ; it++)
			{
				if (FD_ISSET(*it, &writing_set))
				{
					// std::cout << "write" << std::endl;
					long	ret = writeRequest(*it, *_sockets[*it]);

					if (ret == 0) // if the response was entirely written()
					{
						std::cout << GREEN << "\r============= HTTP RESPONSE SENT =============" << RESET << std::endl;
						_ready.erase(it); // erase the socket from the ready set
						
						// Check if we should close the connection
						if (_sockets[*it]->_keepAlive.find(*it) != _sockets[*it]->_keepAlive.end() && !_sockets[*it]->_keepAlive[*it])
						{
							FD_CLR(*it, &_fd_set);
							FD_CLR(*it, &reading_set);
							close(*it);
							_sockets.erase(*it);
						}
					}
					else if (ret == -1)	// if there was an error with write()
					{
						FD_CLR(*it, &_fd_set);
						FD_CLR(*it, &reading_set);
						
						close(*it);
						_sockets.erase(*it);
						_ready.erase(it);
					}
					break;
				}
			}

			// if (select_activity)
			// 	std::cout << "\rReceived a connection !   " << std::endl; //   flush replaces what has been written on the last std_out


			// check read 
			for (std::map<long, MasterSocket *>::iterator it = _sockets.begin() ; select_activity && it != _sockets.end() ; it++)
			{
				long	socket = it->first; // new socket

				if (FD_ISSET(socket, &reading_set))
				{
					// std::cout << "read" << std::endl;
					
					long	readRet = readRequest(socket, *it->second);
					if (readRet == 0)
					{
						std::cout << GREEN << "\r============= HTTP REQUEST RECEIVED =============" << RESET << std::endl;
						it->second->handle(socket, env); // parsing
						_ready.push_back(socket); // add to ready set to write()
					}
					else if (readRet == -1)
					{
						FD_CLR(socket, &_fd_set);
						FD_CLR(socket, &reading_set);
						FD_CLR(socket, &writing_set);
						
						close(socket);	// closing twice
						_sockets.erase(socket);
						it = _sockets.begin();
					}
					break;
				}
			}
			
			// check accept
			for (std::map<long, MasterSocket>::iterator it = _servers.begin() ; select_activity && it != _servers.end() ; it++)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					// std::cout << "accept" << std::endl;
					long	socket = acceptNewConnection(it->second); // get new socket
					if (socket != -1)
					{
						FD_SET(socket, &_fd_set);
						_sockets.insert(std::make_pair(socket, &(it->second)));
						if (socket > _max_fd)
						{
							_max_fd = socket;
						}

					}
					break;
				}
			}
		}
		else
		{
			std::cerr << RED << "ERROR: select returned = " << select_activity << RESET << std::endl;
			for (std::map<long, MasterSocket *>::iterator it = _sockets.begin() ; it != _sockets.end() ; it++)
			{
				if (it->first > 0)
					close(it->first);
				it->second->_requests.erase(it->first);
			}
			_sockets.clear();
			_ready.clear();
			FD_ZERO(&_fd_set);

			_max_fd = 0;
			for (std::map<long, MasterSocket>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
			{
				FD_SET(it->first, &_fd_set);
				if (it->first > _max_fd)
					_max_fd = it->first;
			}
		}
	}
}

bool WebServer::requestCompletelyReceived(std::string completeData, MasterSocket &serv, long socket)
{
	size_t		startPos;
	size_t		endPos;

	size_t headersEndPos = completeData.find("\r\n\r\n");
	if (headersEndPos == std::string::npos)
		return (false);  // Headers aren't fully received yet

	std::string headers = completeData.substr(0, headersEndPos);
	std::string body = completeData.substr(headersEndPos + 4);  // +4 to skip "\r\n\r\n"

	// lower case the headers to find "content-length" and "Content-Length"
	std::string lowerCaseHeaders = toLowerCase(headers);

	size_t conn_pos = lowerCaseHeaders.find("\r\nconnection:");
	if (conn_pos != std::string::npos)
	{
		// Get the value after the "connection:" string.
		size_t start = conn_pos + std::strlen("\r\nconnection:");
		size_t end = lowerCaseHeaders.find("\r\n", start);
		if(end == std::string::npos) // if there's no other header after "connection:"
			end = lowerCaseHeaders.length(); // consider till the end of the string
		
		std::string conn_value = lowerCaseHeaders.substr(start, end - start);

		// Trim whitespace (both leading and trailing)
		conn_value.erase(0, conn_value.find_first_not_of(" \t\n\r"));
		conn_value.erase(conn_value.find_last_not_of(" \t\n\r") + 1);

		if (conn_value == "keep-alive")
			serv._keepAlive[socket] = true;
		else if (conn_value == "close")
			serv._keepAlive[socket] = false;
		// If not specified, and if you're dealing with HTTP/1.1:
		else
			serv._keepAlive[socket] = true; // Default for HTTP/1.1
	}
	else
	{
		// If the header isn't present and you're assuming HTTP/1.1:
		serv._keepAlive[socket] = true; // Default for HTTP/1.1
	}

	// std::cout << "\n HEADER :" << std::endl << "[\n" << YELLOW << headers << RESET << "\n]" << std::endl;
	// std::cout << "\n BODY :" << std::endl << "[\n" << YELLOW << body.substr(0, 1800) << RESET << "\n]" << std::endl;

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
					return (true);
				else if (body.size() > static_cast<size_t>(contentLength))
				{
					serv._requests[socket] = "400";
					std::cerr << RED << "Error with content-length: body > content-length!" << RESET << std::endl;
					return (true);
				}
			}
			catch (const std::exception &e)
			{
				std::cerr << RED << "Error with content-length: not an int!" << RESET << std::endl;
				serv._requests[socket] = "400";
				return (true);
			}
		}
	}

	if (lowerCaseHeaders.find("transfer-encoding: chunked") != std::string::npos)
	{
		if (body.find("0\r\n\r\n") != std::string::npos)
			return (true);
	}

	// No body to receive, just headers
	if (contentLengthPos == std::string::npos && lowerCaseHeaders.find("transfer-encoding:") == std::string::npos)
		return (true);

	// Body not completely received
	return (false);
}


/**
 * Accept new incomming connection.
 * Create new Client object and add it to _client_map
 * Add client socket to _fd_set
*/
long	WebServer::acceptNewConnection(MasterSocket &serv)
{
	struct sockaddr_in	address;
	long				addrlen = sizeof(address);
	long				new_socket;

	new_socket = accept(serv.get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if (new_socket == -1)
	{
		std::cerr << RED << "Could not create socket: " << strerror(errno) << RESET << std::endl;
	}
	else
	{
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		serv._requests.insert(std::make_pair(new_socket, ""));
		std::cout << BLUE << BOLD << "New Connection, Assigned Socket " << new_socket << RESET << std::endl;
	}
	return (new_socket);
}

