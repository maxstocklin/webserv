#include "../../Includes/CgiManager.hpp"

void CgiManager::dispatchResponse(int new_socket, Handler &request, std::string usePath)
{
	if ((usePath.size() > 4 && !usePath.substr(usePath.size() - 4).compare(".php")))
	{
			std::cout << "found .php " << std::endl;
		phpResponder(new_socket, request, usePath);
	}

};

void CgiManager::phpResponder(int new_socket, Handler &request, std::string usePath)

{
	std::cout << "in php responder " << std::endl;
	request.getExecutablePath("php");
    int pipefd[2];
    pipe(pipefd);

	pid_t pid = fork();
	if (pid == 0) {

        close(pipefd[0]);
        int errdup = dup2(pipefd[1], STDOUT_FILENO);

		if (pipefd[1] == -1 || errdup == -1)
		{
			perror("error man");
			exit(0);
		}
		
		char *argv[] =
		{
			const_cast<char*>(request.exec_info.path.c_str()), 
			const_cast<char*>(usePath.c_str()), 
			NULL
		};
		if (request.cgiEnv.empty() || request.cgiEnv.back() != nullptr)
		{
			request.cgiEnv.push_back(nullptr); 
		}

        execve(request.exec_info.path.c_str(), argv, request.cgiEnv.data());
		perror("execve failed");

        exit(0);
	}
	else
	{
		waitpid(pid, NULL, 0);  // Wait for child to finish
		// Parent process code to read from pipe...
	}


    close(pipefd[1]);

    char html_content[4096];
	memset(html_content, 0, sizeof(html_content));

    int readbytes2 = read(pipefd[0], html_content, sizeof(html_content));
	if (readbytes2 == -1)
	{
		perror("buff -1 1");
		exit(1);
	}

	html_content[readbytes2] = 0;
	request.handler_response.statusCode = 200;
	request.handler_response.htmlResponse = html_content;
	request.handler_response.htmlContentType = "text/html";
	request.handler_response.keepAlive = false; // TODO

	// const char *response_headers = 
	// "HTTP/1.1 200 OK\r\n"
	// "Content-Type: text/html\r\n"
	// "Connection: close\r\n";

	// char response[2048];

	// int content_length = strlen(html_content);

	// snprintf(response, sizeof(response), "%sContent-Length: %d\r\n\r\n%s", response_headers, content_length, html_content);
	// std::cout << "---resp\n" << response << "----\n";

	// if (write(new_socket, response, strlen(response)) == -1)
	// {
	// 	perror("buff -1 2");
	// 	exit(1);
	// }

};