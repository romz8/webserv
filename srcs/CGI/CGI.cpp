/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:34 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/29 18:41:02 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//HARDCODED A LOT OF ENV VARIABLES, NEED TO BE CHANGED -> with Merger on Config Parsing
CGI::CGI(Request &req, std::string execpath) : _path(req.getParsePath()), _body(req.getBody()), _execPath(execpath)
{
	_env["AUTH_TYPE"] = "";
	this->_env["SCRIPT_NAME"] = _path;
	this->_env["SCRIPT_FILENAME"] = _path;
	_env["CONTENT_LENGTH"] = std::to_string(_body.size());
	_env["CONTENT_TYPE"] = req.getHeaderField("Content-Type");
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["PATH_INFO"] = req.getParsePath();
	_env["PATH_TRANSLATED"] = req.getParsePath();
	_env["QUERY_STRING"] = req.getHeaderField("Query"); //to be implemented
	_env["REMOTE_ADDR"] = "0.0.0.0";
	_env["SERVER_NAME"] = "webserv"; // from config
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_PORT"] = "4242"; //FROM CONFIG
	_env["HTTP_USER_AGENT"] = req.getHeaderField("USER_AGENT");
	_env["REDIRECT_STATUS"] = "200";
	_env["REQUEST_METHOD"] = req.getMethod();
	_env["SERVER_SOFTWARE"] = "Webserv/2.0";

	std::cerr << BG_RED << "CGI ENV PATH " << this->_env["SCRIPT_NAME"] << RESET << std::endl;
	//_path = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/cgi-bin/hellocgi.sh";
}

CGI::CGI(CGI const &rhs)
{
	*this = rhs;
}

CGI::~CGI(void)
{}

CGI		&CGI::operator=(CGI const &rhs)
{
	if (this == &rhs)
		return (*this);
	_env = rhs._env;
	_path = rhs._path;
	_body = rhs._body;
	return (*this);
}

char **CGI::setEnvExecve(void)
{
	char **env = new char*[this->_env.size() + 1];
	int				i = 0;

	std::map<std::string, std::string>::const_iterator it = _env.begin();
	
	for (; it != _env.end(); ++it)
	{
		std::string envEntry = it->first + "=" + it->second + '\0';
		env[i] = new char[envEntry.size() + 1];
		env[i] = const_cast<char *>(envEntry.c_str());
	}
	env[i] = NULL;
	return (env);
}
void	CGI::executeCGI()
{
	char **env = setEnvExecve();
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
    	std::cout << "Current working dir: " << cwd << std::endl;
	else
    	std::cerr << "getcwd() error" << std::endl;
	if (pipe(_fdin) == -1 || pipe(_fdout) == -1)
	{
		this->_status = 500;
		return ;
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		this->_status = 500;
		return ;
	}
	if (pid == 0)	
	{
		dup2(_fdin[0], STDIN_FILENO);
		dup2(_fdout[1], STDOUT_FILENO);
		close(_fdin[1]);
		close(_fdout[0]);
		
		const char* args[3] = {_execPath.c_str(), _path.c_str(), NULL};
		execve(_execPath.c_str(),  const_cast<char* const*>(args), env);
		this->_status = 500;
		exit(1);
	}
	close(_fdin[0]);
	close(_fdout[1]);
	write(_fdin[1], _body.c_str(), _body.size());
	close(_fdin[1]);
	
	char buffer[1024];
	std::string newbody;
	int byteRead;
	while ((byteRead = read(_fdout[0], buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[byteRead] = '\0';
		newbody += std::string(buffer, byteRead);
	}
	close(_fdout[0]);
	waitpid(pid, NULL, 0);
	_respbody = newbody;
	this->_status = 200;
}
	

int CGI::getStatus(void)
{
	return (_status);
}

std::string CGI::getBody(void)
{
	return (_respbody);
}
