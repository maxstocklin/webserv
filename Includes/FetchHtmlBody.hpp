/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FetchHtmlBody.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 16:11:53 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/13 21:22:00 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FETCHHTMLBODY_HPP
# define FETCHHTMLBODY_HPP
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "./Handler.hpp"

class Handler;

class FetchHtmlBody
{
    public:
        
        static void dispatchResponse(Handler &handler, std::string usePath, std::string mimeType);
        static void phpResponder(Handler &handler, std::string usePath);
        static void htmlResponder(Handler &handler, std::string usePath, std::string mimeType);
        static void lsResponder(Handler &handler, std::string usePath);

};

#endif