#include "../../Includes/FetchHtmlBody.hpp"

void FetchHtmlBody::dispatchResponse(Handler &handler, std::string usePath, std::string mimeType)
{


	if ((usePath.size() > 4 && !usePath.substr(usePath.size() - 4).compare(".php")))
		phpResponder(handler, usePath);
	else if (handler.isFile(usePath))
		htmlResponder(handler, usePath, mimeType);
	else
		lsResponder(handler, usePath);
};

void FetchHtmlBody::phpResponder(Handler &handler, std::string usePath)

{
	handler.getExecutablePath("php");
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe failed");

		// Log the error for diagnostics
		// Return an HTTP 500 error to the client
		std::cout << "DA ERROR 1" << std::endl;
		handler.handler_response.statusCode = 500;
		handler.handler_response.htmlContentType = "text/html";
		return;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		// Log the error for diagnostics
		// Return an HTTP 500 error to the client
		handler.handler_response.statusCode = 500;
		handler.handler_response.htmlContentType = "text/html";
		return;
	}

	if (pid == 0)
	{

		close(pipefd[0]);
		int errdup = dup2(pipefd[1], STDOUT_FILENO);

		if (errdup == -1)
		{
			// TODO PIPE CHECK PERROR
			perror("dup2 failed");
			exit(EXIT_FAILURE);
		}
		
		char *argv[] =
		{
			const_cast<char*>(handler.exec_info.path.c_str()), 
			const_cast<char*>(usePath.c_str()), 
			NULL
		};
		if (handler.cgiEnv.empty() || handler.cgiEnv.back() != nullptr)
			handler.cgiEnv.push_back(nullptr); 

		execve(handler.exec_info.path.c_str(), argv, handler.cgiEnv.data());

		perror("execve failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(pid, NULL, 0);  // Wait for child to finish
		// Parent process code to read from pipe...
	}


	close(pipefd[1]);

	char html_content[4096];
	memset(html_content, 0, sizeof(html_content));

	int readbytes = read(pipefd[0], html_content, sizeof(html_content));
	if (readbytes == -1)
	{
		std::cout << "DA ERROR 2" << std::endl;
		handler.handler_response.statusCode = 500;  // Internal Server Error
		handler.handler_response.htmlContentType = "text/html";
		return;
	}


	html_content[readbytes] = 0;
	handler.handler_response.statusCode = 200;
	handler.handler_response.htmlBody = html_content;
	handler.handler_response.htmlContentType = "text/html";

	if (handler.connection == "keep-alive")
		std::cout << "True" << std::endl;
	else if (handler.connection == "close")
		handler.handler_response.keepAlive = false; // TODO
};

void FetchHtmlBody::htmlResponder(Handler &handler, std::string usePath, std::string mimeType)
{


	int op = 0;
	ssize_t readbytes = 0;
	char buffer[100000]; // consider using a dynamic approach or a larger buffer

	op = open(usePath.c_str(), O_RDONLY);
	if (op == -1)
	{
		const char* errorMessage = strerror(errno);  // Retrieve human-readable error message
		if (errno == ENOENT)
			handler.handler_response.statusCode = 404;  // Not Found
		else if (errno == EACCES)
			handler.handler_response.statusCode = 403;  // Forbidden
		else
			handler.handler_response.statusCode = 500;  // Internal Server Error
		handler.handler_response.htmlContentType = "text/html";
		return;
	}

	readbytes = read(op, buffer, sizeof(buffer));
	if (readbytes == -1)
	{
		handler.handler_response.statusCode = 500;  // Internal Server Error
		handler.handler_response.htmlContentType = "text/html";
		return;
	}


	// Use the buffer directly without adding a null terminator
	std::string imageData(buffer, readbytes);

	handler.handler_response.statusCode = 200;
	handler.handler_response.htmlBody = imageData;
	handler.handler_response.htmlContentType = mimeType;
	if (handler.connection == "keep-alive")
		handler.handler_response.keepAlive = true; // TODO
	else if (handler.connection == "close")
		handler.handler_response.keepAlive = false; // TODO

};

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>

void FetchHtmlBody::lsResponder(Handler &handler, std::string usePath)
{


	std::vector<std::string> result;
	DIR* dir = opendir(usePath.c_str());

	if (dir)
	{
		struct dirent* ent;
		while ((ent = readdir(dir)) != NULL)
			result.push_back(ent->d_name);
		closedir(dir);
	}
	else 
	{
		// std::cerr << "Could not open usePath: " << usePath << std::endl;
		handler.handler_response.statusCode = 500;  // Internal Server Error
		handler.handler_response.htmlContentType = "text/html";
		return;
	}

	std::string html;

	html += "<html><head><title>Directory Listing</title></head><body>";
	html += "<h1>Directory Listing</h1>";
	html += "<ul>";


	for (std::vector<std::string>::const_iterator it = result.begin(); it != result.end(); ++it)
	{
		const std::string& file = *it;
		html += "<li>" + file + "</li>";
	}

	html += "</ul></body></html>";


	handler.handler_response.statusCode = 200;
	handler.handler_response.htmlBody = html;
	handler.handler_response.htmlContentType = "text/html";
	if (handler.connection == "keep-alive")
		handler.handler_response.keepAlive = true; // TODO
	else if (handler.connection == "close")
		handler.handler_response.keepAlive = false; // TODO


};