/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/13 16:27:29 by srapopor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HANDLER_HPP
# define HANDLER_HPP

#include <map>
#include <vector>
#include <iostream>
#include "ListeningSocket.hpp"
#include "CgiManager.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Handler {

    public:
        
        Handler();
        ~Handler();
        void parse(ListeningSocket *master_socket, char **env);
        void setBuffer(char * buffer);
        std::map<std::string, std::string> getParameters(std::string params);
        char _buffer[30000];
        std::string method;
        std::string path;
        std::string fullLocalPath;
        std::string base_index;
        std::string query_string;
        std::string filename;
        std::string connection;
        int contentLength;
        std::vector<std::string> cgiEnvVector;
        std::vector<char *> cgiEnv;
        void getExecutablePath(std::string command);
        void makeFullLocalPath(ListeningSocket *master_socket);
        void getPathResponse(ListeningSocket *master_socket, int new_socket);
        struct exec_info_t {
            std::string path ;
            bool found;
        } exec_info;
        
        struct handler_response_t {
            int statusCode;
            std::string htmlResponse;
            bool keepAlive;
            std::string htmlContentType;
        } handler_response;

    private:

};

std::ostream &operator << (std::ostream &o, Handler  & pr );


#endif