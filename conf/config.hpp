/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 11:12:40 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/08 14:25:52 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <vector>
 #include <fcntl.h>
 
class Config
{
	public:
		// constructors
		Config(char **av);
		Config();
		~Config();

		void readsplit(char *config_file);

		// getters

		//setters

	private:
		int	fd;
		int	brackets_count;
		char buffer[30000];

};



#endif