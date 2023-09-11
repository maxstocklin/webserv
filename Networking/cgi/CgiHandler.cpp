#include "CgiHandler.hpp"
#include <sys/wait.h>

CgiHandler::CgiHandler(Request &req) : _body(req.getBody())
{
	// parse ENV here


	// WIP ServerName hostname - remoteAddr ?
	this->_env["SERVER_NAME"] = ;

	this->_env["SERVER_SOFTWARE"] = "42Webserv/1.0";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_PORT"] = ;
	this->_env["REQUEST_METHOD"] = ;
	this->_env["PATH_INFO"] = ;
	this->_env["PATH_TRANSLATED"] = ;
	this->_env["SCRIPT_NAME"] = ;
	this->_env["QUERY_STRING"] = ;
	this->_env["REMOTE_ADDR"] = ;

	//WIP (?) No Auth Scheme determined ? then set Auth Sheme to "Authorization"
	this->_env["AUTH_TYPE"] = ;
	this->_env["REMOTE_USER"] = ;
	this->_env["REMOTE_IDENT"] = ;
	this->_env["CONTENT_TYPE"] = ;
	this->_env["CONTENT_LENGTH"] = ;
	// this->_env["REQUEST_URI"] = request.getPath() + request.getQuery();
}

CgiHandler::~CgiHandler(void) {}

CgiHandler	&CgiHandler::operator=(CgiHandler const &src) {
	if (this != &src) {
		this->_body = src._body;
		this->_env = src._env;
	}
	return *this;
}


std::string const&CgiHandler::ExecCgi(std::string const & script)
{
	pid_t		pid;
	std::string res;
	// first reunite all env in a char**
	char		**env;
        // // Set environment variables
        // for (std::map<std::string, std::string>::const_iterator it = _env.begin(); it != _env.end(); ++it) {
        //     std::string env_var = it->first + "=" + it->second;
        //     putenv(const_cast<char*>(env_var.c_str()));
        // }
	int pipe_fd[2];
	int status;

	if(pipe(pipe_fd) == -1)
	{
		//handle pipe error
		return res;
	}

	// fork a process to execute logic implementation
	pid = fork();
	if (pid == -1)
	{
		// handle fork error 
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return res;
	}

	if(pid == 0) // here we'r in the child process
	{
		close(pipe_fd[0]);  // Close the read end of the pipe
        // Redirect standard output to the write end of the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            // Handle error
            exit(1);
        }
        // Execute the CGI script
        execve(script.c_str(), NULL, env);
        // If execve fails, handle the error
        exit(1);
	}
	else {
		waitpid(pid, &status, 0);

	}


	// return string response body
	return res;
}
