/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/10/04 22:59:08 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/MasterSocket.hpp"

MasterSocket::MasterSocket(std::string serverBlock) : BindingSocket(serverBlock)
{
	backlog = 10000; // find an other way
	start_listening();
	test_connection(listening);

	// ensure that the master_socket socket operates in non-blocking mode
	test_connection(fcntl(get_sock(), F_SETFL, O_NONBLOCK)); // possible to add FD_CLOEXEC arg too, which closes automatically the fd when execve is called
}

void MasterSocket::start_listening()
{
	listening = listen(get_sock(), backlog);
	struct timeval timeout;
	timeout.tv_sec = 5;  // 5 seconds timeout
	timeout.tv_usec = 0;

	// Set receive timeout
	if (setsockopt(get_sock(), SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throw std::runtime_error("Error: setsockopt() timeout");

	// Set send timeout
	if (setsockopt(get_sock(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throw std::runtime_error("Error: setsockopt() timeout");

	std::cout << std::endl << GREEN << BOLD << "################################# NEW MASTER SOCKET #################################" << RESET << std::endl;
	std::cout << "socket fd = " << get_sock() << std::endl;
	std::cout << "Endpoint / IP:port comination = " << get_endpoint() << std::endl << std::endl;
	
}



void MasterSocket::parseChunks(long socket)
{
	size_t headersEndPos = _requests[socket].find("\r\n\r\n");

	std::string headers = _requests[socket].substr(0, headersEndPos);
	std::string lowerCaseHeaders = toLowerCase(headers);
	std::string body = _requests[socket].substr(headersEndPos + 4);  // +4 to skip "\r\n\r\n"

	if (lowerCaseHeaders.find("transfer-encoding: chunked") != std::string::npos)
	{
		size_t		i = 0;
		std::string	_bodyString;

		while (i < body.size())
		{
			// Parse the chunk size
			size_t chunkSize = 0;

			while (i < body.size() && body[i] != '\r')
			{
				chunkSize = chunkSize * 16 + (isxdigit(body[i]) ? (isdigit(body[i]) ? body[i] - '0' : tolower(body[i]) - 'a' + 10) : 0);
				i++;
			}
			i += 2; // Skip the CRLF after the chunk size

			// Append the chunk data to _bodyString
			_bodyString.append(body, i, chunkSize);

			i += chunkSize + 2; // Skip the chunk data and the following CRLF
		}
		_requests[socket] = headers + "\r\n\r\n" + _bodyString + "\r\n\r\n";
	}
}

// at this point, the entire request was fetched, and is not parsed
// request[socket] is the correct request
void		MasterSocket::handle(long socket, char **env)
{
	Response		response(env);

	if (_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos &&
		_requests[socket].find("Transfer-Encoding: chunked") < _requests[socket].find("\r\n\r\n"))
		this->parseChunks(socket); // manage chunked data and remove hexadecimals from body

	if (PRINT) // set to 1 to print the request
	{
		if (_requests[socket].size() < 1000)
			std::cout << "\nRequest :" << std::endl << "[" << YELLOW << _requests[socket] << RESET << "]" << std::endl;
		else
			std::cout << "\nRequest :" << std::endl << "[" << YELLOW << _requests[socket].substr(0, 1000) << "..." << _requests[socket].substr(_requests[socket].size() - 10, 15) << RESET << "]" << std::endl;
	}

	if (_requests[socket] != "")
	{
		Request			request(_requests[socket]);

		if (request.getRet() != 200)
			request.setMethod("GET");

		response.call(request, *this, _keepAlive[socket]);

		// remove the fetched http request string
		_requests.erase(socket);
		// Replace it with the complete http response string
		_requests.insert(std::make_pair(socket, response.getResponse()));
	}
}

const std::string	&MasterSocket::get_path() const
{
	return (this->fullLocalPath);
}

void	MasterSocket::set_path(std::string path)
{
	this->fullLocalPath = path;
}
