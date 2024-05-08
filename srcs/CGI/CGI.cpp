/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:34 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/08 22:20:19 by rjobert          ###   ########.fr       */
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
	_env["PATH_INFO"] = req.getPath();
	_env["PATH_TRANSLATED"] = req.getPath();
	_env["QUERY_STRING"] = req.getQueryString();
	_env["HTTP_COOKIE"] = req.getHeaderField("Cookie");
	std::cout << BG_YELLOW "QUERY STRING : " << req.getQueryString() << RESET << std::endl;
	std::cout << BG_YELLOW "QUERY STRING in env: " << _env["QUERY_STRING"] << RESET << std::endl;
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
		std::string envEntry = it->first + "=" + it->second + "\0";
		env[i] = new char[envEntry.size() + 1];
		env[i] = std::strcpy(env[i], envEntry.c_str());
		i++;
	}
	env[i] = NULL;
	return (env);
}
void	CGI::executeCGI()
{
	char **env = setEnvExecve();
	for (int i = 0; env[i]; i++)
		std::cerr << BG_GREEN "env is : " << env[i] << RESET << std::endl;
	if (!isExecutable(_execPath))
	{
		this->_status = 500;
		return ;
	}
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
		std::cout << BG_YELLOW "ARRIVE HERE 1" << RESET << std::endl;
		dup2(_fdin[0], STDIN_FILENO);
		dup2(_fdout[1], STDOUT_FILENO);
		close(_fdin[1]);
		close(_fdout[0]);
		
		const char* args[3] = {_execPath.c_str(), _path.c_str(), NULL};
		execve(_execPath.c_str(),  const_cast<char* const*>(args), env);
		this->_status = 502;
		exit(1);
	}
	close(_fdin[0]);
	close(_fdout[1]);
	write(_fdin[1], _body.c_str(), _body.size());
	close(_fdin[1]);
	
	char buffer[1024];
	std::string newbody;
	int byteRead;
	std::cout << BG_YELLOW "ARRIVE HERE 2" << RESET << std::endl;
	while ((byteRead = read(_fdout[0], buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[byteRead] = '\0';
		newbody += std::string(buffer, byteRead);
	}
	std::cout << BG_YELLOW "ARRIVE HERE 3" << RESET << std::endl;
	close(_fdout[0]);
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		_respbody = newbody;
		this->_status = 200;
	}
	else
		this->_status = 502;
	
	for (int i = 0; env[i]; i++)
		delete env[i];
	delete env;
}
	

int CGI::getStatus(void)
{
	return (_status);
}

std::string CGI::getBody(void)
{
	return (_respbody);
}

void CGI::checkCGI()
{
	if (_respbody.empty() || _respbody.find("Content-Type:") == std::string::npos)
	{	
		_status = 502;
		std::cerr << BG_RED "CGI response is empty -> 502 direct" RESET << std::endl;
		return;
	}
	size_t headerEnd = _respbody.find("\r\n\r\n");
	int delimSize = 4;
	if (headerEnd == std::string::npos)
	{
		headerEnd = _respbody.find("\n\n");
		std::cout << BG_YELLOW "HEADER END IS : " << headerEnd << RESET << std::endl;
		delimSize = 2;
	}
	if( headerEnd == std::string::npos)
	{
		std::cerr << BG_RED "CGI response is not well formatted" RESET << std::endl;
		_status = 502;
		return;
	}
	std::string header = _respbody.substr(0, headerEnd);
	std::string body = _respbody.substr(headerEnd + delimSize);
	std::cout << BG_YELLOW "HEADER IS : " << header << " and delim is " << delimSize << RESET << std::endl;
	std::cout << BG_YELLOW "BODY IS : " << body << RESET << std::endl;
	if (header.find("Content-Length:") == std::string::npos)
	{
		std::cerr << BG_RED "Content-Length is not present" RESET << std::endl;
		int	len = body.length();
		std::ostringstream oss;
		oss << "Content-Length: " << len << "\r\n";
		_respbody = oss.str() + _respbody;
		//_respbody = header + "\r\n" + cl + "\r\n" + body;
		return;
	}
}

bool isExecutable(const std::string& path) 
{
	std::cout << BG_YELLOW "PATH IS : " << path << RESET << std::endl;
	struct stat info;
    return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IXUSR);
}