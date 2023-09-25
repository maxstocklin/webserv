/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:54:48 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/25 21:08:16 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/MasterSocket.hpp"

MasterSocket::MasterSocket(std::string serverBlock) : BindingSocket(serverBlock)
{
	backlog = 3; // find an other way
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

void MasterSocket::parseChunks(long socket)
{
	size_t headersEndPos = _requests[socket].find("\r\n\r\n");

	if (headersEndPos == std::string::npos)
	{
		// TODO: error code: the final request received is incorrect
	}

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

	// TODO: ERROR CHECK
	// if (_bodyString.length() > _server.get_client_max_body_size())
	// {
	// 	set_response_status_code(413);
	// }
}

// at this point, the entire request was fetched, and is not parsed
// request[socket] is the correct request
void		MasterSocket::handle(long socket, char **env)
{
	Response		response(env);

	// it->second->makeFullLocalPath(server);	--> done
	// it->second->getPathResponse(server, new_socket);
	// it->second->parseRequest(socket, env); // socket is used to fetch the right _requests

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
			request.setMethod("GET");	// TODO: find why, probably to avoid some unecessary parsing of the body

		response.call(request, *this); // todo: send *this instead of requestConf

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



// TODO: FIND OUT WHAT TODO, now it's just empty strings
std::string	MasterSocket::getCgiPass()
{
	return "";
}

std::map<std::string, std::string>	MasterSocket::getCgiParam()
{
	std::map<std::string, std::string> yolo;
	return (yolo);

}

