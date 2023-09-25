/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndexGenerator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 22:39:12 by max               #+#    #+#             */
/*   Updated: 2023/09/25 00:15:49 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef AUTO_INDEX_GENERATOR_HPP

# define AUTO_INDEX_GENERATOR_HPP

#include "Colors.hpp"

#include <stdio.h>
#include <iostream>
#include <cstring>

#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <list>

#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>

#include <sys/types.h>
#include <dirent.h>

class AutoIndexGenerator
{
    public:
        AutoIndexGenerator(void);
        AutoIndexGenerator(AutoIndexGenerator const &src);
        virtual ~AutoIndexGenerator(void);

        AutoIndexGenerator   &operator=(AutoIndexGenerator const &src);

        static std::string  getPage(const char *path, std::string const &host, int port);
    private:
        static std::string  getLink(std::string const &dirEntry, std::string const &dirName, std::string const &host, int port);

};

#endif
