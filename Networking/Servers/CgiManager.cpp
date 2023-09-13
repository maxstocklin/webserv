#include "../../Includes/CgiManager.hpp"

void CgiManager::dispatchResponse(Handler &request, std::string usePath, std::string mimeType)
{
	if ((usePath.size() > 4 && !usePath.substr(usePath.size() - 4).compare(".php")))
	{
		phpResponder(request, usePath);
	}
	
	else if (request.isFile(usePath))
	{
		htmlResponder(request, usePath, mimeType);
	}
	else
	{
		lsResponder(request, usePath);
	}
};

void CgiManager::phpResponder(Handler &request, std::string usePath)

{
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


};

void CgiManager::htmlResponder(Handler &request, std::string usePath, std::string mimeType)
{
    int op = 0;
    ssize_t readbytes = 0;
    char buffer[100000]; // consider using a dynamic approach or a larger buffer

    op = open(usePath.c_str(), O_RDONLY);
    if (op < 0) {
        request.handler_response.statusCode = 404;
        return;
    }

    readbytes = read(op, buffer, sizeof(buffer));
    close(op); // Always close the file

    if (readbytes < 0) {
        request.handler_response.statusCode = 500; // 500 Internal Server Error might be more appropriate than 404
        return;
    }

    // Use the buffer directly without adding a null terminator
    std::string imageData(buffer, readbytes);

    request.handler_response.statusCode = 200;
    request.handler_response.htmlResponse = imageData;
    request.handler_response.htmlContentType = mimeType;
    request.handler_response.keepAlive = false;
};

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>

void CgiManager::lsResponder(Handler &request, std::string usePath)
{

    std::vector<std::string> result;
    DIR* dir = opendir(usePath.c_str());

    if (dir) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != NULL) {
            result.push_back(ent->d_name);
        }
        closedir(dir);
    } else {
        // Handle error
        std::cerr << "Could not open usePath: " << usePath << std::endl;
    }

	request.handler_response.statusCode = 200;
	// request.handler_response.htmlResponse = html_content;
	request.handler_response.htmlContentType = "text/html";
	request.handler_response.keepAlive = false; // TODO


    std::string html;

    html += "<html><head><title>Directory Listing</title></head><body>";
    html += "<h1>Directory Listing</h1>";
    html += "<ul>";


	for (std::vector<std::string>::const_iterator it = result.begin(); it != result.end(); ++it) {
		const std::string& file = *it;
		html += "<li>" + file + "</li>";
	}

    html += "</ul></body></html>";


	request.handler_response.statusCode = 200;
	request.handler_response.htmlResponse = html;
	request.handler_response.htmlContentType = "text/html";
	request.handler_response.keepAlive = false; // TODO


};