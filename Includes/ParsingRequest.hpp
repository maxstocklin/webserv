/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/11 18:10:41 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSINGREQUEST_HPP
# define PARSINGREQUEST_HPP

#include <map>
#include <vector>
#include <iostream>
#include "ListeningSocket.hpp"

class ParsingRequest {

    public:
        
        ParsingRequest();
        ~ParsingRequest();
        void parse(ListeningSocket *master_socket);
        void setBuffer(char * buffer);
        std::map<std::string, std::string> getParameters(std::string params);
        char _buffer[30000];
        std::map<std::string, std::string> urlParams;
        std::map<std::string, std::string> bodyParams;
        std::string method;
        std::string path;
        std::string query_string;
        std::string filename;
        std::string connection;
        int contentLength;
        std::vector<std::string> cgiEnvVector;
        std::vector<char *> cgiEnv;

    private:

};

std::ostream &operator << (std::ostream &o, ParsingRequest  & pr );


#endif