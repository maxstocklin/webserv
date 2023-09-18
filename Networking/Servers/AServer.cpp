/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 15:29:07 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/18 12:55:20 by srapopor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/AServer.hpp"


AServer::AServer(char *config_file)
{
	// split the conf file into server {} blocks 
	// and insert them in std::vector<std::string> serverBlocks
	split_server_blocks(config_file);

	// Create a new socket for each server{} block
	for (size_t i = 0; i < serverBlocks.size(); ++i)
	{
		sockets.push_back(new ListeningSocket(serverBlocks[i]));
	}
}

AServer::~AServer()
{
    for (size_t i = 0; i < sockets.size(); ++i)
	{
        delete sockets[i];
    }
}

ListeningSocket * AServer::get_socket(int i)
{
	return (sockets[i]);
}

std::vector<ListeningSocket*> AServer::get_socket_vector()
{
	return (sockets);
}


void AServer::split_server_blocks(const char *config_file)
{
	std::ifstream configFile(config_file);
	if (!configFile.good() || !configFile.is_open())
		throw std::runtime_error(std::string("Error opening file: ") + config_file);

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	
	// Remove comments
	std::string configContent = removeCommentLines(buffer.str());

	std::string::size_type startPos = 0, endPos = 0;
	
	while ((startPos = configContent.find("server", startPos)) != std::string::npos)
	{
		endPos = startPos + 6;  // move past "server"

		while ((configContent[endPos] == ' ' || configContent[endPos] == '\n' || configContent[endPos] == '\t') && endPos < configContent.size())
			endPos++;

		if (!configContent[endPos] || configContent[endPos] != '{')
			throw std::runtime_error("Error: wrong config file format");

		int bracketCount = 1;
		endPos++;
		
		while (bracketCount > 0 && endPos < configContent.size())
		{
			if (configContent[endPos] == '{')
				bracketCount++;
			else if (configContent[endPos] == '}')
				bracketCount--;
			endPos++;
		}
		
		// add the server{} block to the vector if bracketCount == 0
		// otherwise send an error
		if (bracketCount == 0)
			serverBlocks.push_back(configContent.substr(startPos, endPos - startPos));
		else
			throw std::runtime_error("Error: Missing ending brackets '}' in config file!");

		configContent.erase(startPos, endPos - startPos);
	}

	// check if there is something else remaining in the config file
	if (configContent.find_first_not_of(" \t\n\r\f\v") != std::string::npos)
			throw std::runtime_error("Error: Only specify config inside server { } blocks!");
}

std::string AServer::removeCommentLines(const std::string &input)
{
	std::stringstream ss(input);
	std::stringstream output;
	std::string line;
	while (std::getline(ss, line))
	{
		// Remove leading spaces
		size_t start = line.find_first_not_of(" \t");
		if (start != std::string::npos)
			line = line.substr(start);
		else
			line.clear();
		
		// Check if line starts with # after leading spaces are removed
		if (!line.empty() && line[0] != '#')
			output << line << "\n";
	}
	return output.str();
}