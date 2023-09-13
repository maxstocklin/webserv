/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 16:08:01 by max               #+#    #+#             */
/*   Updated: 2023/09/13 21:14:20 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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


int main(int ac, char **av, char **env)
{
	
    if (fork() == 0) // Child process
	{
        // close(pipefd[0]); // Close read end of pipe
        // dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe


		char *argv[] = {
			const_cast<char*>("/usr/bin/php"),  // Path to PHP interpreter
			const_cast<char*>("/Users/mstockli/cursus/webss/Networking/Cgi/index.php"),  // Path to your PHP script
			NULL
		};
		char *envp[] = { const_cast<char*>("QUERY_STRING=value"), NULL };

		std::cout << "########################################\n\n" << std::endl;

        execve("/usr/bin/php", argv, envp); // Run PHP on the script
		std::cout << "########################################\n\n" << std::endl;
		perror("execve failed");

        exit(0);
    }
}





/*
- add more error codes and pages
- add ls command
- add post and delete
- change request by handler
- make the program able to receive jpeg png, txt, html
- change he structure from contentResponse to contentBody

*/image