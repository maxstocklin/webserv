#include "CgiHandler.hpp"

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
	// fork a process to execute logic implementation


	// return string response body
	return res;
}
