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
		handler.set_response_status_code(500);
		handler.set_response_htmlContentType("text/html");
		return;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		// Log the error for diagnostics
		// Return an HTTP 500 error to the client
		handler.set_response_status_code(500);
		handler.set_response_htmlContentType("text/html");
		return;
	}

	if (pid == 0)
	{

  		dup2(pipefd[1], STDOUT_FILENO);
      	int errdup =  dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
	

		if (errdup == -1)
		{
			// TODO PIPE CHECK PERROR
			perror("dup2 failed");
			exit(EXIT_FAILURE);
		}

		exec_info_t execInfo = handler.get_exec_info();

		char *argv[] =
		{
			const_cast<char*>(execInfo.path.c_str()), 
			const_cast<char*>(usePath.c_str()), 
			NULL
		};
		if (handler.get_cgiEnv().empty() || handler.get_cgiEnv().back() != nullptr)
			handler.get_cgiEnv().push_back(nullptr);
		// std::cout << " path " << handler.get_exec_info().path.c_str() << std::endl;
		// std::cout << " path " << usePath.c_str() << std::endl;

		execve(handler.get_exec_info().path.c_str(), argv, handler.get_cgiEnv().data());

		perror("execve failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		// close(pipefd[0]);
        write(pipefd[1],  handler.get_imageData().c_str(), handler.get_imageData().size());
        close(pipefd[1]);
		waitpid(pid, NULL, 0);  // Wait for child to finish
		// Parent process code to read from pipe...
	}


	close(pipefd[1]);

	char html_content[4096];
	memset(html_content, 0, sizeof(html_content));

	int readbytes = read(pipefd[0], html_content, sizeof(html_content));
	std::cout << "html content" << html_content << std::endl;
	if (readbytes == -1)
	{
		handler.set_response_status_code(500);  // Internal Server Error
		handler.set_response_htmlContentType("text/html");
		return;
	}


	html_content[readbytes] = 0;
	handler.set_response_status_code(200);
	handler.set_response_htmlBody(html_content);
	handler.set_response_htmlContentType("text/html");

	if (handler.get_connection() == "keep-alive")
		handler.set_response_keepAlive(true);
	else if (handler.get_connection() == "close")
		handler.set_response_keepAlive(false);
};

void FetchHtmlBody::htmlResponder(Handler &handler, std::string usePath, std::string mimeType)
{
	int op = 0;
	ssize_t readbytes = 0;
	char buffer[10000]; // consider using a dynamic approach or a larger buffer

	op = open(usePath.c_str(), O_RDONLY);
	if (op == -1)
	{
		if (errno == ENOENT)
			handler.set_response_status_code(404);  // Not Found
		else if (errno == EACCES)
			handler.set_response_status_code(403);  // Forbidden
		else
			handler.set_response_status_code(500);  // Internal Server Error
		handler.set_response_htmlContentType("text/html");
		return;
	}

	readbytes = read(op, buffer, sizeof(buffer));
	if (readbytes == -1)
	{
		handler.set_response_status_code(500);  // Internal Server Error
		handler.set_response_htmlContentType("text/html");
		return;
	}


	// Use the buffer directly without adding a null terminator
	std::string imageData(buffer, readbytes);

	handler.set_response_status_code(200);
	handler.set_response_htmlBody(imageData);
	handler.set_response_htmlContentType(mimeType);
	if (handler.get_connection() == "keep-alive")
		handler.set_response_keepAlive(true);
	else if (handler.get_connection() == "close")
		handler.set_response_keepAlive(false);

};

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
		handler.set_response_status_code(500);  // Internal Server Error
		handler.set_response_htmlContentType("text/html");
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

	handler.set_response_status_code(200);
	handler.set_response_htmlBody(html);
	handler.set_response_htmlContentType("text/html");
	if (handler.get_connection() == "keep-alive")
		handler.set_response_keepAlive(true);
	else if (handler.get_connection() == "close")
		handler.set_response_keepAlive(false);
};
