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