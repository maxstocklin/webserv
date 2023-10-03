/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:05 by mstockli          #+#    #+#             */
/*   Updated: 2023/10/03 20:49:17 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP


#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <sstream>

bool						isNumber(const std::string &s);
bool						isValidPort(int port);
bool						endsWithSemicolon(const std::string &str);
bool						isValidHost(const std::string& host);
std::string					trimWhiteSpaces(const std::string &str);
std::vector<std::string>	splitToVector(const std::string &str);

std::string					toLowerCase(const std::string& input);
void						*ft_memcpy(void *dst, const void *src, size_t n);
std::string					&pop(std::string& str);
std::string					&to_upper(std::string& str);
bool 						compare_langs(const std::pair<std::string, float> first, const std::pair<std::string, float> second);
std::string						readValue(const std::string& line);
int								ft_atoi(const char *str);
void							ft_skip_spacenl(const char *str, int *i);
std::string						readValue(char *line);
std::string						readKey(const std::string& line);
std::string						readKey(char *line);
std::string&					strip(std::string& str, char c);
std::vector<std::string>		split(const std::string& str, char c);
std::string&					capitalize(std::string& str);
std::string&					to_lower(std::string& str);

std::string						to_string(size_t n);


#endif