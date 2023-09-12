/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/12 16:08:52 by srapopor         ###   ########.fr       */
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
        void parse(ListeningSocket *master_socket, char **env);
        void setBuffer(char * buffer);
        std::map<std::string, std::string> getParameters(std::string params);
        char _buffer[30000];
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