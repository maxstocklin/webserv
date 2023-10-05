/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/03 20:16:07 by max               #+#    #+#             */
/*   Updated: 2023/10/05 23:45:29 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../Includes/Utils.hpp"

// check if the string is a valid number
bool isNumber(const std::string &str)
{
	if (str.empty())
		return (false);  // ensures the string is not empty
	
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!isdigit(*it))
			return (false);
	}
	return (true);
}

// check if the port is valid (must be a int between 0 and 65535)
bool isValidPort(int port)
{
	if (port >= 0 && port <= 65535)
		return (true);
	return (false);
}

// check if the line ends with a semicolon
bool endsWithSemicolon(const std::string &str)
{
	if (!str.empty() && str.back() == ';')
		return (true);
	return (false);
}

// remove all the tabs and spaces at the extremeties of the string
std::string trimWhiteSpaces(const std::string &str)
{
	std::size_t first = str.find_first_not_of(" \t");

	if (std::string::npos == first) // if the string is only made of white spaces, return the string
		return (str);

	std::size_t last = str.find_last_not_of(" \t");

	return (str.substr(first, (last - first + 1)));
}

// check if the host is valid (e.g. 127.0.0.1)
bool isValidHost(const std::string& host)
{
	std::istringstream	ss(host);
	std::string			segment;
	int					count = 0; // count number of dots

	// split into segments separated by a dot and check if it's a valid int between 0 and 255
	while (getline(ss, segment, '.'))
	{
		if (++count > 4)
			return (false);

		int num = std::atoi(segment.c_str());
		if (num < 0 || num > 255)
			return (false);

		// make sure no extra characters exist in the segment
		for (std::string::iterator it = segment.begin(); it != segment.end(); ++it)
		{
			if (!isdigit(*it)) 
				return (false);
		}
	}

	if (count == 4)
		return (true);
	return (false);
}

// Split a string into a vector for each word
std::vector<std::string> splitToVector(const std::string &str)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, ' '))  // Split based on spaces first
	{
		if (!token.empty())
		{
			std::stringstream ss2(token);
			std::string subToken;
			
			while (std::getline(ss2, subToken, '\t'))  // Then split based on tabs
			{
				if (!subToken.empty()) 
					tokens.push_back(subToken);
			}
		}
	}

	return tokens;
}


std::string toLowerCase(const std::string& input)
{
	std::string result = input;
	for (std::string::iterator it = result.begin(); it != result.end(); ++it)
	{
		*it = std::tolower(static_cast<unsigned char>(*it));
	}
	return result;
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
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

std::string	&pop(std::string& str)
{
	if (str.size())
		str.resize(str.size() - 1);
	return str;
}


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

std::string&					to_upper(std::string& str)
{
	std::transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}

std::string	to_string(size_t n)
{
	std::stringstream tmp;

	tmp << n;

	return tmp.str();
}