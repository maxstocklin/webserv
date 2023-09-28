/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/28 01:43:00 by max              ###   ########.fr       */
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

// add to utils
static void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*p;
	unsigned char	*q;

	i = 0;
	p = (unsigned char *)dst;
	q = (unsigned char *)src;
	while (i < n)
	{
		p[i] = q[i];
		i++;
	}
	return (dst);
}



void	WebServer::initializeSets()
{
	FD_ZERO(&_fd_set);
	_max_fd = 0;
	
	// adds servers sockets to _fd_set set
	for(std::vector<MasterSocket>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		addToSet(it->get_sock(), _fd_set);
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


bool	WebServer::readRequest(long socket, MasterSocket &serv)
{
	int		bytes_read = 0;
	char	buffer[40000];

	memset(buffer, 0, sizeof(buffer));

	bytes_read = read(socket, buffer, sizeof(buffer) - 1);

	if (bytes_read == 0)
	{
		std::cout << YELLOW << BOLD << "webserv: Client " << socket << " Closed Connection" << RESET << std::endl;
		closeConnection(socket);
		// _sockets.erase(socket);
		return (false);
	}
	else if (bytes_read < 0)
	{
		std::cout << RED << BOLD << "webserv: Client " << socket << " read error " << strerror(errno) << RESET << std::endl;
		closeConnection(socket);
		// _sockets.erase(socket);
		return (false);
	}
	else if (bytes_read != 0)
	{
		serv._requests[socket].append(buffer, bytes_read);

		// c.updateTime();
		// _requests[socket].append(buffer, bytes_read);
	}
	return (requestCompletelyReceived(serv._requests[socket]));
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
						_sockets.erase(*it);
						_ready.erase(it);
					}
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
					
					if (readRequest(socket, *it->second)) // returns true if the http request is fully received 
					{
						// std::cout << "request before: \n" << YELLOW << it->second->_requests[it->first] << RESET << std::endl;
						it->second->handle(socket, env); // parsing / todo1
						_ready.push_back(socket); // add to ready set to write()
					}
					break;
				}
			}
			
			// check accept
			for (std::map<long, MasterSocket>::iterator it = _servers.begin() ; select_activity && it != _servers.end() ; it++)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					std::cout << "accept" << std::endl;
					acceptNewConnection(it->second); // get new socket
					break;
				}
			}
		}
		// todo: add error check with timeouts and select = -1

	}

}






// TODO: toLowerCase == utils
static std::string toLowerCase(const std::string& input)
{
	std::string result = input;
	for (std::string::iterator it = result.begin(); it != result.end(); ++it)
	{
		*it = std::tolower(static_cast<unsigned char>(*it));
	}
	return result;
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
	size_t		endPos;

	size_t headersEndPos = completeData.find("\r\n\r\n");
	if (headersEndPos == std::string::npos)
		return (false);  // Headers aren't fully received yet

	std::string headers = completeData.substr(0, headersEndPos);
	std::string body = completeData.substr(headersEndPos + 4);  // +4 to skip "\r\n\r\n"

	// lower case the headers to find "content-length" and "Content-Length"
	std::string lowerCaseHeaders = toLowerCase(headers);

	// aaaaaa cout
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
			std::cout << "content length __ " << lengthStr << std::endl; 
			std::cout << "body.size() __ " << body.size() << std::endl; 

			try
			{
				int contentLength = std::stoi(lengthStr);
				if (body.size() == static_cast<size_t>(contentLength))
					return (true);
				else if (body.size() > static_cast<size_t>(contentLength))
				{
					// todo: set error code, body size is too big
				}
			}
			catch (const std::exception &e)
			{
				// TODO: ERROR CODE?
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

	// No body, just headers
	if (contentLengthPos == std::string::npos && lowerCaseHeaders.find("transfer-encoding:") == std::string::npos)
		return (true);

	// No complete body yet
	return (false);
}


/* Closes connection from fd i and remove associated client object from _clients_map */
void	WebServer::closeConnection(const int i)
{
	if (FD_ISSET(i, &_fd_set))
		removeFromSet(i, _fd_set);
	if (FD_ISSET(i, &_fd_set))
		removeFromSet(i, _fd_set);
	close(i);
	_sockets.erase(i);
}





/**
 * Accept new incomming connection.
 * Create new Client object and add it to _client_map
 * Add client socket to _fd_set
*/
void	WebServer::acceptNewConnection(MasterSocket &serv)
{
	struct sockaddr_in	address;
	long				addrlen = sizeof(address);
	int					new_socket;

	if ((new_socket = accept(serv.get_sock(), (struct sockaddr *)&address, (socklen_t*)&addrlen)) == -1)
	{
		throw std::runtime_error(std::string("Could not create socket!") + strerror(errno));
		return;
	}
	serv._requests.insert(std::make_pair(new_socket, ""));
	std::cout << BLUE << BOLD << "New Connection, Assigned Socket " << new_socket << RESET << std::endl;

	addToSet(new_socket, _fd_set);
	_sockets.insert(std::make_pair(new_socket, &(serv))); // insert new socket to the map of new sockets

	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << RED << BOLD << "webserv: fcntl error " << strerror(errno) <<  RESET << std::endl;
		removeFromSet(new_socket, _fd_set);
		close(new_socket);
		return ;
	}
}


void	WebServer::addToSet(const int i, fd_set &set)
{
	FD_SET(i, &set);
	if (i > _max_fd)
		_max_fd = i;
}

void	WebServer::removeFromSet(const int i, fd_set &set)
{
	FD_CLR(i, &set);
	if (i == _max_fd)
		_max_fd--;
}
