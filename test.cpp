/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 16:08:01 by max               #+#    #+#             */
/*   Updated: 2023/09/11 16:36:03 by max              ###   ########.fr       */
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
			const_cast<char*>("/usr/local/bin/php"),  // Path to PHP interpreter
			const_cast<char*>("/Users/max/Desktop/cursus/webserv3/Networking/Cgi/index.php"),  // Path to your PHP script
			NULL
		};
		std::cout << "########################################\n\n" << std::endl;

        execve("/usr/local/bin/php", argv, env); // Run PHP on the script
		std::cout << "########################################\n\n" << std::endl;
		perror("execve failed");

        exit(0);
    }
}





    int pipefd[2];
    pipe(pipefd); // Create a pipe to capture output

    if (fork() == 0) // Child process
	{
        close(pipefd[0]); // Close read end of pipe
        int errdup = dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe

		if (pipefd[1] == -1 || errdup == -1)
		{
			perror("error man");
			exit(0);
		}
		char *argv[] = {
			const_cast<char*>("/usr/local/bin/php"),  // Path to PHP interpreter
			const_cast<char*>("/Users/max/Desktop/cursus/webserv3/Networking/Cgi/index.php"),  // Path to your PHP script
			NULL
		};
		std::cout << "########################################\n\n" << std::endl;
	    close(pipefd[1]); // Close write end of pipe in the parent
		fflush(stdout);

        execve("/usr/local/bin/php", argv, NULL); // Run PHP on the script
		std::cout << "########################################\n\n" << std::endl;
		perror("execve failed");

        exit(0);
    }

    close(pipefd[1]); // Close write end of pipe in the parent

    char buff2[4096];
    int readbytes2 = read(pipefd[0], buff2, sizeof(buff2)); // Read the script's output

	buff2[readbytes2] = 0;
	std::cout << " Read bytes = " << readbytes2 << "buf = " << buff2 << std::endl;

	write(new_socket, buff2, strlen(buff2));

