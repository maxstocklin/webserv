/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srapopor <srapopor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 22:40:13 by max               #+#    #+#             */
/*   Updated: 2023/10/05 16:16:39 by srapopor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/CgiHandler.hpp"

CgiHandler::CgiHandler(Request &request, MasterSocket &config) : _body(request.getBody())
{
	this->_initEnv(request, config);
}

CgiHandler::CgiHandler(CgiHandler const &src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_env = src._env;
	}
	return ;
}

CgiHandler::~CgiHandler(void)
{
}

CgiHandler	&CgiHandler::operator=(CgiHandler const &src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_env = src._env;
	}
	return *this;
}


// create the env for execve
void		CgiHandler::_initEnv(Request &request, MasterSocket &config)
{

	std::map<std::string, std::string>	headers = request.getHeaders();
	if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
		this->_env["AUTH_TYPE"] = headers["Authorization"];

	this->_env["REDIRECT_STATUS"] = "200"; //Security needed to execute php-cgi
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = config.get_path();
	this->_env["SCRIPT_FILENAME"] = config.get_path();
	this->_env["REQUEST_METHOD"] = request.getMethod();
	this->_env["CONTENT_LENGTH"] = to_string(this->_body.length());
	this->_env["CONTENT_TYPE"] = headers["Content-Type"];
	this->_env["PATH_INFO"] = request.getPath(); //might need some change, using config path/contentLocation
	this->_env["PATH_TRANSLATED"] = request.getPath(); //might need some change, using config path/contentLocation
	this->_env["QUERY_STRING"] = request.getQuery();
	this->_env["REMOTEaddr"] = to_string(config.get_host_int());
	this->_env["REMOTE_IDENT"] = headers["Authorization"];
	this->_env["REMOTE_USER"] = headers["Authorization"];
	this->_env["REQUEST_URI"] = request.getPath() + request.getQuery();
	if (headers.find("Hostname") != headers.end())
		this->_env["SERVER_NAME"] = headers["Hostname"];
	else
		this->_env["SERVER_NAME"] = this->_env["REMOTEaddr"];
	this->_env["SERVER_PORT"] = to_string(config.get_port());
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "WebServ/1.0";

	if (request.getMethod() == "POST")
	{
		this->_env["SCRIPT_NAME"] = "./Networking/Cgi/post.php";
		this->_env["SCRIPT_FILENAME"] = "./Networking/Cgi/post.php";
	}
}

char	**CgiHandler::_getEnvAsCstrArray() const
{
	char	**env = new char*[this->_env.size() + 1];
	int	j = 0;
	for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++)
	{
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	env[j] = NULL;
	return env;
}



std::string		CgiHandler::executeCgi(const std::string& scriptName)
{
	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	// std::cout << "this->_env[CONTENT_TYPE] = " << this->_env["CONTENT_TYPE"] << std::endl;
	// std::cout << "this->_env[CONTENT_LENGTH] = " << this->_env["CONTENT_LENGTH"] << std::endl;


	try
	{
		env = this->_getEnvAsCstrArray();
		// int i = 0;
		// while (env[i])
		// {
		// 	std::cout << "env i = " << env[i] << std::endl;
		// 	i++;
		// }
		// std::cout << "scriptName = " << scriptName << std::endl;
		// std::cout << "this->_env[SCRIPT_NAME] = " << this->_env["SCRIPT_NAME"] << std::endl;

	}
	catch (std::bad_alloc &e)
	{
		std::cout << "cerr " << std::endl;
		std::cerr << RED << e.what() << RESET << std::endl;
	}

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	int pipeOut[2];  // pipe for output

    if ( pipe(pipeOut) == -1)
    {
        std::cerr << "Failed to create pipes" << std::endl;
        return ("Status: 500\r\n\r\n");
    }




	const char* tmpFilePath = "/tmp/request_body.txt";
    int tmpFileFd = open(tmpFilePath, O_RDWR | O_CREAT );
	if (tmpFileFd == -1)
		std::cout << RED << "OPEN ERROR" << RESET << std::endl;
	// std::cout << "\nRequest :" << std::endl << "[" << YELLOW << _body.substr(0, 800) <<  RESET << "]" << std::endl;

	write(tmpFileFd, _body.c_str(), _body.size());
	close(tmpFileFd);
	tmpFileFd = open(tmpFilePath, O_RDWR | O_CREAT );

	pid = fork();
	
	if (pid == -1)
	{
		std::cerr << RED << "Fork crashed." << RESET << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (pid == 0)
	{
		char *argv[] =
		{
			const_cast<char*>(scriptName.c_str()),
			const_cast<char*>(this->_env["SCRIPT_NAME"].c_str()),
			NULL
		};

		dup2(tmpFileFd, STDIN_FILENO);
		close(tmpFileFd);
		close(pipeOut[0]); // Close read end of output pipe

        dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeOut[1]);


		execve(scriptName.c_str(), argv, env);

		std::cerr << RED << "Execve crashed." << RESET << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else
	{
		char	buffer[CGI_BUFSIZE] = {0};

		waitpid(-1, NULL, 0);
  		close(pipeOut[1]);
		while (true)
		{
			memset(buffer, 0, sizeof(buffer));
			int readbytes = read(pipeOut[0], buffer, sizeof(buffer) - 1);

			if (readbytes <= 0)
				break;
			newBody += buffer;
		}
	}
	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	close(pipeOut[0]);
  
	close(tmpFileFd);
	close(saveStdin);
	close(saveStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);
	return (newBody);
}