/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 11:12:32 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/08 14:28:10 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

Config::Config(char **av)
{
	readsplit(av[1]);
}

Config::Config()
{


}

~Config::Config()
{


}

void Config::readsplit(char *config_file)
{
	brackets_count = 0;
	memset(buffer, 0, sizeof(buffer));
	int readbytes = 1;
	
	try
	{
		fd = open(config_file, O_RDWR);
		if (fd < 0)
			throw std::runtime_error("Error: ");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	while (readbytes > 0)
	{
		readbytes = read(fd, buffer, sizeof(buffer) - 1);
		
	}
	

}