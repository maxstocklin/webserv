/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 11:34:27 by srapopor          #+#    #+#             */
/*   Updated: 2023/09/06 16:56:07 by srapopor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSINGREQUEST_HPP
# define PARSINGREQUEST_HPP

#include <map>
#include<iostream>

class ParsingRequest {

    public:
        
        ParsingRequest();
        ~ParsingRequest();
        void parse();
        void setBuffer(char * buffer);
        void getUrlParameters(std::string params);
        void getBodyParameters(std::string params);
        char _buffer[30000];
        std::map<std::string, std::string> urlParams;
        std::map<std::string, std::string> bodyParams;
        std::string method;
        std::string path;
        std::string connection;
        int contentLength;
   
    private:

};


#endif