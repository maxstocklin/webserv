/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndexGenerator.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 22:40:08 by max               #+#    #+#             */
/*   Updated: 2023/09/24 22:40:09 by max              ###   ########.fr       */
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

std::string         AutoIndexGenerator::getPage(const char *path, std::string const &host, int port) {
    std::string dirName(path);
    DIR *dir = opendir(path);
    std::string page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + dirName + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>INDEX</h1>\n\
    <p>\n";

    if (dir == NULL) {
        std::cerr << RED << "Error: could not open [" << path << "]" << RESET << std::endl;
        return "";
    }
    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
        page += AutoIndexGenerator::getLink(std::string(dirEntry->d_name), dirName, host, port);
    }
    page +="\
    </p>\n\
    </body>\n\
    </html>\n";
    closedir(dir);
    return page;
}

std::string         AutoIndexGenerator::getLink(std::string const &dirEntry, std::string const &dirName, std::string const &host, int port) {
    std::stringstream   ss;
    ss << "\t\t<p><a href=\"http://" + host + ":" <<\
        port << dirName + "/" + dirEntry + "\">" + dirEntry + "</a></p>\n";
    return ss.str();
}
