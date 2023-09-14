/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 16:08:01 by max               #+#    #+#             */
/*   Updated: 2023/09/14 04:11:48 by max              ###   ########.fr       */
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
- add keep alive
- finish adding chunks

*/

#include <cerrno> // for errno
#include <fcntl.h> // for open
#include <cstring> // for strerror

int fd = open("somepath", O_RDONLY);
if (fd == -1) {
    if (errno == ENOENT) {
        // Handle "Not Found" scenario (e.g., return 404 status)
    } else if (errno == EACCES) {
        // Handle "Permission Denied" scenario (e.g., return 403 status)
    } else {
        // Handle other errors (e.g., return 500 status)
        // Optionally, you can use strerror(errno) to get a human-readable error message.
    }
}
