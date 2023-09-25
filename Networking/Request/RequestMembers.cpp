/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:18:10 by max               #+#    #+#             */
/*   Updated: 2023/09/24 22:04:58 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Request.hpp"

// TODO: UTILS


std::string&					to_lower(std::string& str)
{
	std::transform(str.begin(), str.end(),str.begin(), ::tolower);
	return str;
}

std::string&					capitalize(std::string& str)
{
	size_t	i = 0;

	to_lower(str);
	str[i] = std::toupper(str[i]);
	while((i = str.find_first_of('-', i + 1)) != std::string::npos)
	{
		if (i + 1 < str.size())
		str[i + 1] = std::toupper(str[i + 1]);
	}
	return str;
}

std::vector<std::string>		split(const std::string& str, char c)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, c))
		tokens.push_back(token);
	return tokens;
}

static std::string&					pop(std::string& str)
{
	if (str.size())
		str.resize(str.size() - 1);
	return str;
}

std::string&					strip(std::string& str, char c) // trim
{
	size_t	i;

	if (!str.size())
		return str;
	i = str.size();
	while (i && str[i - 1] == c)
		i--;
	str.resize(i);
	for (i = 0; str[i] == c; i++);
	str = str.substr(i, std::string::npos);
	return str;
}

std::string						readKey(char *line)
{
	std::string	ret;

	for (int i = 0; line[i] && line[i] != ':'; i++)
		ret.push_back(line[i]);
	capitalize(ret);
	return (strip(ret, ' '));
}

std::string						readKey(const std::string& line)
{
	std::string	ret;

	size_t	i = line.find_first_of(':');
	ret.append(line, 0 , i);
	capitalize(ret);
	return (strip(ret, ' '));
}

std::string						readValue(char *line)
{
	int i;
	std::string	ret;

	for (i = 0; line[i] && line[i] != ':'; i++);
	while (line[++i] && line[i] == ' ');
	i--;
	while (line[++i])
		ret.push_back(line[i]);
	return (strip(ret, ' '));
}


void	ft_skip_spacenl(const char *str, int *i)
{
	while ((str[*i] == ' ' || str[*i] == '\t' || str[*i] == '\n')
	|| (str[*i] == '\r' || str[*i] == '\v' || str[*i] == '\f'))
		(*i)++;
}

int	ft_atoi(const char *str)
{
	int	i;
	int	sign;
	int	num;

	i = 0;
	sign = 1;
	num = 0;
	ft_skip_spacenl(str, &i);
	if (str[i] == '-')
		sign = -1;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
		num = num * 10 + (str[i++] - '0');
	return (num * sign);
}


std::string						readValue(const std::string& line)
{
	size_t i;
	std::string	ret;

	i = line.find_first_of(':');
	i = line.find_first_not_of(' ', i + 1);
	if (i != std::string::npos)
		ret.append(line, i, std::string::npos);
	return (strip(ret, ' ')); // trim leading and tailing spaces
}


bool compare_langs(const std::pair<std::string, float> first, const std::pair<std::string, float> second)
{
  return ( first.second > second.second );
}
std::string&					to_upper(std::string& str)
{
	std::transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}








/// END OF TODO UTILS















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
	this->_headers["Accept-Language"] = "";
	this->_headers["Allow"] = "";
	this->_headers["Auth-Scheme"] = "";
	this->_headers["Authorization"] = "";
	this->_headers["Content-Language"] = "";
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
	return this->readPath(line, i);
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
	// std::cout << "\nPATH = " << _path << std::endl;
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
		// TODO1: case insensivity
		if (key.find("Secret") != std::string::npos)
			this->_env_for_cgi[formatHeaderForCGI(key)] = value; // create a new map element, with <HTTP_ + KEY, value>
	}
	if (this->_headers["Www-Authenticate"] != "")
		this->_env_for_cgi["Www-Authenticate"] = this->_headers["Www-Authenticate"];
	// this->setLang(); // REMOVED

	// set the _body attribute and trim the tailing \r\n chars
	this->setBody(str.substr(i, std::string::npos));

	this->findQuery();
	return this->_ret;
}

/*
**	"Cet en-tête est une indication destinée à être utilisée lorsque le serveur
**	n'a aucun moyen de déterminer la langue d'une autre manière, comme une URL
**	spécifique, qui est contrôlée par une décision explicite de l'utilisateur.
**	Il est recommandé que le serveur ne passe jamais outre une décision explicite."
*/

void				Request::setLang() // USELESS
{
	std::vector<std::string>	token;
	std::string					header;
	size_t						i;

	if ((header = this->_headers["Accept-Language"]) != "")
	{
		token = split(header, ',');
		for (std::vector<std::string>::iterator it = token.begin(); it != token.end(); it++)
		{
			float			weight = 0.0;
			std::string		lang;

			lang = (*it).substr(0, (*it).find_first_of('-'));
			strip(lang, ' ');
			if ( (i = lang.find_last_of(';') ) != std::string::npos)
			{
				weight = atof( (*it).substr(i + 4).c_str() );
			}
			lang.resize(i > 2 ? 2 : i);
			this->_lang.push_back(std::pair<std::string, float>(lang, weight));
		}
		this->_lang.sort(compare_langs);
	}
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
	for (size_t i = 0 ; i < key.size() ; i++) {
		if (key[i] == '-')
			key[i] = '_';
	}
	return "HTTP_" + key;
}
