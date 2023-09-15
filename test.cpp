/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 16:08:01 by max               #+#    #+#             */
/*   Updated: 2023/09/15 10:10:12 by max              ###   ########.fr       */
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
			const_cast<char*>("Networking/Cgi/index.php"),  // Path to your PHP script
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

- add post and delete
- add keep alive // almost done
- finish adding chunks
- handle all read with GNL


Create a checker function before the html body fetch
- is content length > max body size

*/

