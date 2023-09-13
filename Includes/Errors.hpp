/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:30:23 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/13 12:53:46 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
# define ERRORS_HPP

#include <stdio.h>
#include <iostream>
#include <cstring>

#include <stdio.h> 
#include <map>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <fcntl.h>
class Errors
{
	public:
		Errors(std::map<int, std::string> errorMap);
		~Errors();

	std::string get_error_content(int statusCode);

	private:
		std::map<int, std::string> errorMap;
		std::map<int, std::string> statusMessage;
};

#endif