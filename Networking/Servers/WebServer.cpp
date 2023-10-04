/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/10/04 17:38:12 by mstockli         ###   ########.fr       */
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
		// addToSet(it->get_sock(), _fd_set);
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
			close(socket);
		serv._requests.erase(socket);

		if (!bytes_read)
			std::cout << YELLOW << BOLD << "webserv: Client " << socket << " Closed Connection" << RESET << std::endl;
		else
			std::cerr << RED << BOLD << "webserv: Client " << socket << " read error " << strerror(errno) << RESET << std::endl; // todo: erno after read??
		return (-1);
	}

	// if (bytes_read == 0)
	// {
	// 	std::cout << YELLOW << BOLD << "webserv: Client " << socket << " Closed Connection" << RESET << std::endl;
	// 	FD_CLR(socket, &_fd_set);
	// 	FD_CLR(socket, &reading_set); // damn
	// 	_sockets.erase(socket);
	// 	// closeConnection(socket);
	// 	return (false);
	// }
	// else if (bytes_read < 0)
	// {
	// 	std::cout << RED << BOLD << "webserv: Client " << socket << " read error " << strerror(errno) << RESET << std::endl;
	// 	FD_CLR(socket, &_fd_set);
	// 	FD_CLR(socket, &reading_set); // damn
	// 	_sockets.erase(socket);
	// 	// closeConnection(socket);
	// 	return (false);
	// }
	else if (bytes_read != 0)
	{
		serv._requests[socket].append(buffer, bytes_read);
	}
	
	if (requestCompletelyReceived(serv._requests[socket]) == true)
		return 0;
	else
		return 1;
	// return (requestCompletelyReceived(serv._requests[socket]));
}


long	WebServer::writeRequest(long socket, MasterSocket &serv)
{
	static std::map<long, size_t>	sent;

	if (sent.find(socket) == sent.end()) // check if something was already sent
		sent[socket] = 0;

	if (PRINT == 12 && sent[socket] == 0) // print the response if nothing was already sent in chunk
	{
		if (serv._requests[socket].size() < 1000)
			std::cout << "\rResponse :                " << std::endl << "[" << GREEN << serv._requests[socket] << RESET << "]\n" << std::endl;
		else
			std::cout << "\rResponse :                " << std::endl << "[" << GREEN << serv._requests[socket].substr(0, 1000) << "..." << serv._requests[socket].substr(serv._requests[socket].size() - 10, 15) << RESET << "]\n" << std::endl;
	}

	// send data by chunks of size RECV_SIZE = 30000
	std::string	str = serv._requests[socket].substr(sent[socket], 30000); // create a string from what was already sent (if any --> sent[socket] != 0) until either the end of the response of max RECV_SIZE size
	int	ret = write(socket, str.c_str(), str.size()); // write the string

	if (ret == -1) // if an error occured with write()
	{
		// closeConnection(socket);
		if (socket > 0)
			close(socket);

		serv._requests.erase(socket);
		// this->close(socket); // close the new socket
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
			return (0);
		}
		else	// if part of the response was written
			return (1);
	}
}

void	WebServer::launch()
{
	initializeSets();

	while (42)
	{
		fd_set			reading_set;
		fd_set			writing_set;
		int				select_activity = 0;
		struct timeval	timeout;

		std::cout << "\r============= WAITING FOR NEXT CONNECT =============" << std::endl;
		while (select_activity == 0)
		{
			timeout.tv_sec  = 1;
			timeout.tv_usec = 0;
			ft_memcpy(&reading_set, &_fd_set, sizeof(_fd_set));
			FD_ZERO(&writing_set);		// reset writing set
			for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)	// add every new socket that is ready to the writing set
				FD_SET(*it, &writing_set);

			std::cout << "select max fd = " << _max_fd << std::endl;

			std::cout << "reading_set = ";
			for (int i = 0; i <= _max_fd; i++) {
				if (FD_ISSET(i, &reading_set)) {
					std::cout << i << " ";
				}
			}
			std::cout << "writing_set = ";
			std::cout << std::endl;
			for (int i = 0; i <= _max_fd; i++) {
				if (FD_ISSET(i, &writing_set)) {
					std::cout << i << " ";
				}
			}
			std::cout << std::endl;
			
			for (int i = 0; i <= _max_fd; i++) {
				if (FD_ISSET(i, &_fd_set)) {
					std::cout << "Socket in _fd_set: " << i << std::endl;
				}
				else if (i > 500)
				{
					// shutdown(i, SHUT_WR);
					close(i);
				}
			}

			select_activity = select(_max_fd + 1, &reading_set, &writing_set, NULL, &timeout); //
		}
		
		if (select_activity > 0)
		{
			// check write
			for (std::vector<int>::iterator it = _ready.begin() ; select_activity && it != _ready.end() ; it++)
			{
				if (FD_ISSET(*it, &writing_set))
				{
					std::cout << "write" << std::endl;
					long	ret = writeRequest(*it, *_sockets[*it]);

					if (ret == 0) // if the response was entirely written()
						_ready.erase(it); // erase the socket from the ready set
					else if (ret == -1)	// if there was an error with write()
					{
						// closeConnection(socket);
						FD_CLR(*it, &_fd_set);
						FD_CLR(*it, &reading_set);
						// shutdown(*it, SHUT_WR);
						close(*it);
						_sockets.erase(*it);
						_ready.erase(it);
					}
					ret = 0;
					break;
				}
			}

			if (select_activity)
				std::cout << "\rReceived a connection !   " << std::endl; //   flush replaces what has been written on the last std_out


			// check read 
			for (std::map<long, MasterSocket *>::iterator it = _sockets.begin() ; select_activity && it != _sockets.end() ; it++)
			{
				long	socket = it->first; // new socket

				if (FD_ISSET(socket, &reading_set))
				{
					std::cout << "read" << std::endl;
					
					long	readRet = readRequest(socket, *it->second);
					if (readRet == 0)
					{
						it->second->handle(socket, env); // parsing
						_ready.push_back(socket); // add to ready set to write()
					}
					else if (readRet == -1)
					{
						FD_CLR(socket, &_fd_set);
						FD_CLR(socket, &reading_set);
						// shutdown(socket, SHUT_WR);
						close(socket);
						_sockets.erase(socket);
						it = _sockets.begin();
					}

					// if (readRequest(socket, *it->second)) // returns true if the http request is fully received 
					// {
					// 	it->second->handle(socket, env); // parsing
					// 	_ready.push_back(socket); // add to ready set to write()
					// }
					readRet = 0;
					break;
				}
			}
			
			// check accept
			for (std::map<long, MasterSocket>::iterator it = _servers.begin() ; select_activity && it != _servers.end() ; it++)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					std::cout << "accept" << std::endl;
					long	socket = acceptNewConnection(it->second); // get new socket
					if (socket != -1)
					{
						// if (socket > 1023)
						// 	exit (0);
						std::cout << "1 socket is " << socket << " && max fd is " << _max_fd << std::endl;
						FD_SET(socket, &_fd_set);
						std::cout << "2 socket is " << socket << " && max fd is " << _max_fd << std::endl;
						_sockets.insert(std::make_pair(socket, &(it->second)));
						if (socket > _max_fd)
						{
							_max_fd = socket;
							std::cout << "NEW MAX FD CHANGED TO " << _max_fd << std::endl;
						}
						for (int i = 0; i <= _max_fd; i++)
						{
							if (FD_ISSET(i, &_fd_set))
							{
								std::cout << YELLOW << "Socket in _fd_set after ACCEPT: " << RESET << i << std::endl;
							}
						}

					}
					// select_activity = 0;
					break;
				}
			}
		}
		else
		{
			perror("select");  
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
			// for (std::map<long, MasterSocket>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
			// 	FD_SET(it->first, &_fd_set);

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

bool WebServer::requestCompletelyReceived(std::string completeData)
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
					std::cout << "EXIT 1" << std::endl;
					exit(0);
					// TODO: ERROR CODE 400
				}
			}
			catch (const std::exception &e)
			{
					std::cout << "EXIT 2" << std::endl;
				exit(0);
				// TODO: ERROR CODE 400
				// TODO: if the file is too big, it's not an int --> crash
				std::cerr << e.what() << '\n';
				throw std::runtime_error("Error with content-length --> not an int" + lengthStr);
				return (false);
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

