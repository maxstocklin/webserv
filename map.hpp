/*

classes : 

	MasterSocket

	ServerSocket: public MasterSocket

	ClientSocket: public MasterSocket


ClientSocket waiting for connect from client

		Create ClientSocket -> request content -> parsing
			IsReady

loop()
{
	FD_ISSET()
		if (fd tested is ready)
			handler for this fd
			response for this fd
}

*/

/*
    Server initialization:
        Initialize server socket, socket sets, and other necessary variables.
			struct sockaddr_in address; //struct used for speciying socket addresses in ipv4 communication. Also contains sin_zero for padding but not often used.
			address.sin_family = AF_INET; //sets the family address of the socket (AF_INET == ipv4)
			address.sin_addr.s_addr = INADDR_ANY; //sets the IP address (INADDR_ANY == the socket will accept incoming connections on any available network interface or IP address on the server)
			address.sin_port = htons( 80 ); //sets the port number to 80 (function important for portability, because it converts the port number from the host byte order to the network byte order if necessary)


    select() Setup:
        Set up the fd_set to monitor sockets.
			fd_set readfds; //data type used for managing sets of SDs, used with FD_SET(), FD_CLR(), FD_ISSET(), etc... it is an array of bits, each bit representing the status (ready or not) of a SD
			FD_ZERO(&readfds); //init the set to 0;
			//
        Add the server socket to the set.
			FD_SET(get_socket()->get_sock(), &readfds);
			max_sd = get_socket()->get_sock();

        Determine the maximum socket descriptor value (max_sd) for select().

    select() Call:
        Call select() to wait for socket activity.
        select() will block until one or more sockets are ready for I/O.

    Check Server Socket:
        Check if the server socket is ready for activity using FD_ISSET(server_socket, &readfds).
        If it is ready, accept a new connection (accepter()).
        Optionally, you can add the new client socket to the list of client sockets for future monitoring.

    Loop Through Client Sockets:
        Loop through the client sockets (those in the client_socket array) using a for loop.

    Check Client Socket Ready:
        For each client socket, check if it is ready for reading using FD_ISSET(client_socket[i], &readfds).

    Handle Incoming Data:
        If a client socket is ready for reading, read the incoming data (request) using recv().
        Process the incoming request and perform any necessary server logic (e.g., handler()).

    Send Response:
        Send a response back to the client using send() or other relevant response mechanisms (e.g., responder()).

    Cleanup:
        If a client socket is closed or the connection is terminated, close the socket, remove it from the set, and mark it as available for reuse (client_socket[i] = 0).

    Repeat:
        Return to the beginning of the loop to continue monitoring sockets for activity.
        Continue this loop indefinitely to handle multiple client connections concurrently.

    Error Handling:
        Handle errors that may occur during socket operations or within the server logic.
        Log error messages or take appropriate actions based on the error.

The order is structured to efficiently handle multiple client connections using a single-threaded server. It monitors sockets for activity, accepts new connections, processes incoming requests, sends responses, and cleans up closed sockets as necessary. This pattern allows your server to handle multiple clients concurrently while using a single thread of execution.
*/

/* 
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) <sys/select.h>


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

   // Watch stdin (fd 0) to see when it has input.
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

   // Wait up to five seconds.
    tv.tv_sec = 5;
    tv.tv_usec = 0;

   retval = select(1, &rfds, NULL, NULL, &tv);
    // Don't rely on the value of tv now!

   if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        // FD_ISSET(0, &rfds) will be true. 
    else
        printf("No data within five seconds.\n");

   exit(EXIT_SUCCESS);
}

ACCEPT
First connection in the queue, creates a new socket for it, returns a file descriptor
The original socket requesting connection is unchanged. The newly created socket is not listening
Must be called for the socket opened with socket(), bound to a local address with bind() and 
is listening to connections after a listen()



*/
	fd_set readfds;
	int highest_sd = server_socket_sd;
	int sockets[];
	int ready_socket;
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( 80 );

	FD_ZERO(&readfds);

	FD_SET(server_socket_sd, &readfds);

	sockets[0] = server_socket_sd;

	while (1)
	{
		std::cout << "Waiting for network activity" << std::endl;
		if (select(highest_sd + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			std::cout << "error during the select call" << std::endl;
			break ;//for now, but we won't exit here if an error occurs.
		}
		for (int i = 0; sockets[i]; i++)
		{
			// Returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor set pointed to by fdset, and 0 otherwise.
			ready_socket = FD_ISSET(sockets[i], &readfds);
			// FD_ISSET returns the server_socket as ready, a client asks for a connection, and at least one spot is left in the set, we need to accept the connection, and add the new_sd to the set to be monitored with select.
			if (ready_socket == server_socket_sd)
			{
				std::cout << "a new connection is handled" << std::endl;
				int new_sd = accept();
				FD_SET(new_sd, &readfds);
				if (new_sd > highest_sd)// I guess that solve this case : we have 3 fds, 0, 1 ans 2. fd 1 was closed, the new_sd should logically be 1, so we don't need to change the highest sd.
					highest_sd = ready_socket;
				// what happens if the highest_sd is closed ? Is it ok to send a not monitored sd as the highest_sd in the select() funcion call ?
				for (int j = 0; sockets[j]; j++)
				{
					if (sockets[j] == 0)
						sockets[j] = new_sd;
				}
			}
			// FD_ISSET returns a ready_socket different than the server socket, so we need to handle the execution (in a fork ?)
			else if (ready_socket >= 0)
			{
				std::cout << "a socket is ready" << std::endl;
				int bytesread = read(sockets[i], buffer, 1024);
				if (bytesread == 0)
				{
					std::cout << "nothing to read" << std::endl;
					// depending on the header, might keep alive the connection after this.
					// if this is empty, is it because everything was read already ? Or the request was empty so it is an error, hopefully an empty read here does not mean bad stuff is about to happen to us...
					buffer[bytesread] = 0;//NULL terminated string
					send(sockets[i], strlen(buffer), 0);// The content read in the request received by the client is sent back without any modification to the string.
				}
			}
		}
	}
