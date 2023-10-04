/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:18:10 by max               #+#    #+#             */
/*   Updated: 2023/10/03 23:27:42 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Request.hpp"

void				Request::displayHeaders() const
{
}

std::ostream&		operator<<(std::ostream& os, const Request& re)
{
	std::map<std::string, std::string>::const_iterator	it;

	os << "Method : " << re.getMethod() << " |\tHTTP version : ";
	os << re.getVersion() << '\n';
	os << "Port : " << re.getPort() << '\n';
	os << "Path : " << re.getPath() << '\n';

	for (it = re.getHeaders().begin(); it != re.getHeaders().end(); it++)
		os << it->first << ": " << it->second << '\n';

	os << '\n' << "Request body :\n" << re.getBody() << '\n';

	return os;
}

void				Request::resetHeaders()
{
	this->_headers.clear();

	this->_headers["Accept-Charsets"] = "";
	this->_headers["Allow"] = "";
	this->_headers["Auth-Scheme"] = "";
	this->_headers["Authorization"] = "";
	this->_headers["Content-Length"] = "";
	this->_headers["Content-Location"] = "";
	this->_headers["Content-Type"] = "";
	this->_headers["Date"] = "";
	this->_headers["Host"] = "";
	this->_headers["Last-Modified"] = "";
	this->_headers["Location"] = "";
	this->_headers["Referer"] = "";
	this->_headers["Retry-After"] = "";
	this->_headers["Server"] = "";
	this->_headers["Transfer-Encoding"] = "";
	this->_headers["User-Agent"] = "";
	this->_headers["Www-Authenticate"] = "";
	this->_headers["Connection"] = "Keep-Alive";
}

int					Request::readFirstLine(const std::string& str)
{
	size_t	i;
	std::string	line;

	i = str.find_first_of('\n');
	line = str.substr(0, i);
	i = line.find_first_of(' ');

	if (i == std::string::npos)
	{
		this->_ret = 400;
		std::cerr << RED << "RFL no space after method" << RESET << std::endl;
		return 400;
	}
	this->_method.assign(line, 0, i);
	to_upper(this->_method);
	return this->readPath(line, i);
}

std::string decodeSpaces(const std::string& input)
{
    std::string result;
    for (std::size_t i = 0; i < input.size(); ++i)
	{
        if (i < input.size() - 2 && input.substr(i, 3) == "%20")
		{
            result += ' ';
            i += 2;  // skip the next two characters
        }
		else
		{
            result += input[i];
        }
    }
    return result;
}


int					Request::readPath(const std::string& line, size_t i)
{
	size_t	j;

	if ((j = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		this->_ret = 400;
		std::cerr << RED << "No PATH / HTTP version" << RESET << std::endl;
		return 400;
	}
	if ((i = line.find_first_of(' ', j)) == std::string::npos)
	{
		this->_ret = 400;
		std::cerr << RED << "No HTTP version" << RESET << std::endl;
		return 400;
	}
	this->_path.assign(line, j, i - j);
	_path = decodeSpaces(_path);
	return this->readVersion(line, i);
}

int					Request::readVersion(const std::string& line, size_t i)
{
	if ((i = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		this->_ret = 400;
		std::cerr << RED << "No HTTP version" << RESET << std::endl;
		return 400;
	}
	if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' &&
			line[i + 3] == 'P' && line[i + 4] == '/')
		this->_version.assign(line, i + 5, 3);
	if (this->_version != "1.0" && this->_version != "1.1")
	{
		this->_ret = 400;
		std::cerr << RED << "BAD HTTP VERSION (" << this->_version << ")" << RESET << std::endl;
		return (this->_ret);
	}
	return (this->checkMethod());
}

int					Request::checkMethod()
{
	for (size_t i = 0; i < this->methods.size(); i++)
		if (this->methods[i] == this->_method)
			return this->_ret;
	std::cerr << RED << "Invalid method requested" << RESET << std::endl;
	this->_ret = 400;
	return this->_ret;
}

int					Request::checkPort()
{
	size_t i = this->_headers["Host"].find_first_of(':');

	if (i == std::string::npos)
		this->_port = 80;
	else
	{
		std::string tmp(this->_headers["Host"], i + 1);
		this->_port = ft_atoi(tmp.c_str());
	}
	return (this->_port);
}

std::string			Request::nextLine(const std::string &str, size_t& i)
{
	std::string		ret;
	size_t			j;

	if (i == std::string::npos)
		return "";
	j = str.find_first_of('\n', i);
	ret = str.substr(i, j - i);
	if (ret[ret.size() - 1] == '\r')
		pop(ret);
	i = (j == std::string::npos ? j : j + 1);
	return ret;
}


int					Request::parse(const std::string& str)
{
	std::string		key;
	std::string		value;
	std::string		line;
	size_t			i(0);

	this->readFirstLine(nextLine(str, i));
	while ((line = nextLine(str, i)) != "\r" && line != "" && this->_ret != 400) // parse headers line by line
	{
		// handle each header line which are structured as follows: "Key: Value"
		key = readKey(line); // get capitalized 'KEY'
		value = readValue(line); // get 'Value' as received
		if (this->_headers.count(key)) // check in the headers map if there is such data a key and set it
				this->_headers[key] = value;

		if (key.find("secret") != std::string::npos)
			this->_env_for_cgi[formatHeaderForCGI(key)] = value; // create a new map element, with <HTTP_ + KEY, value>
	}
	if (this->_headers["www-authenticate"] != "")
		this->_env_for_cgi["www-authenticate"] = this->_headers["www-authenticate"];

	// set the _body attribute and trim the tailing \r\n chars
	this->setBody(str.substr(i, std::string::npos));

	this->findQuery();
	return this->_ret;
}


void				Request::stripAll()
{
	strip(this->_method, '\n');
	strip(this->_method, '\r');
	strip(this->_method, ' ');
	strip(this->_version, '\n');
	strip(this->_version, '\r');
	strip(this->_version, ' ');
	strip(this->_path, '\n');
	strip(this->_path, '\r');
	strip(this->_path, ' ');
}

void				Request::findQuery()
{
	size_t		i;

	// at this point, _path includes the ? variables
	i = this->_path.find_first_of('?');

	// if there are some ? query variables, remove them from _path and save them to _query
	if (i != std::string::npos)
	{
		this->_query.assign(this->_path, i + 1, std::string::npos);
		this->_path = this->_path.substr(0, i);
	}
}

std::string 		Request::formatHeaderForCGI(std::string& key)
{
	to_upper(key);
	for (size_t i = 0 ; i < key.size() ; i++)
	{
		if (key[i] == '-')
			key[i] = '_';
	}
	return "HTTP_" + key;
}
