/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndexGenerator.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 22:40:08 by max               #+#    #+#             */
/*   Updated: 2023/10/03 23:02:21 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/AutoIndexGenerator.hpp"

AutoIndexGenerator::AutoIndexGenerator(void) {
	return ;
}

AutoIndexGenerator::AutoIndexGenerator(AutoIndexGenerator const &src) {
	(void)src;
	return ;
}

AutoIndexGenerator::~AutoIndexGenerator(void) {
	return ;
}

AutoIndexGenerator	&AutoIndexGenerator::operator=(AutoIndexGenerator const &src) {
	(void)src;
	return *this;
}

std::string	AutoIndexGenerator::getPage(const char *path, std::string const &host, int port, std::string basicUri) {
	std::string dirName(path);
	DIR *dir = opendir(path);
	std::string page =\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<meta charset=\"UTF-8\">\n\
			<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
			<title>" + dirName + "</title>\n\
			<link rel=\"stylesheet\" href=\"/css/styles.css\">\n\
	</head>\n\
	<body>\n\
		<div class=\"container\">\n\
		<header>\n\
			<h1>INDEX</h1>\n\
			<h2>Check this Directory Listing, bro:</h2>\n\
		</header>\n\
		\n\
		<nav>\n\
			<ul class=\"vertical-list\">\n";

	if (dir == NULL)
	{
		std::cerr << RED << "Error: could not open [" << path << "]" << RESET << std::endl;
		return "";
	}
	if (dirName[0] != '/')
		dirName = "/" + dirName;
	for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
	{
		if (std::string(dirEntry->d_name) != "." && std::string(dirEntry->d_name) != "..")
			page += AutoIndexGenerator::getLink(std::string(dirEntry->d_name), dirName, host, port, basicUri);

	}
	page +="\
						<li><a href=\"/\">BACK HOME</a></li>\n\
				</ul>\n\
			</nav>\n\
		</div>\n\
	</body>\n\
	</html>\n";
	closedir(dir);
	std::cout << YELLOW << page << RESET << std::endl;
	return page;
}

std::string	AutoIndexGenerator::getLink(std::string const &dirEntry, std::string const &dirName, std::string const &host, int port, std::string basicUri)
{
	std::stringstream   ss;

	(void)dirName;
	// Ensure the basicUri ends with a '/' before appending dirEntry
	if (basicUri[basicUri.length() - 1] != '/')
	{
		basicUri += "/";
	}
	std::string betterHost = "127.0.0.1";
	(void)host;
	ss << "\t\t\t\t\t<li><a href=\"http://" + betterHost + ":" <<\
		port << basicUri + dirEntry + "\">" + dirEntry + "</a></li>\n";

	return ss.str();
}
