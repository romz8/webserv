/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReqCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 19:10:29 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/29 15:29:12 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

void Request::initCgi(std::string execPath) {
    _cgi._path = getParsePath();
    _cgi._body = getBody();
    _cgi._execPath = execPath;
    _cgi._env["AUTH_TYPE"] = "";
    _cgi._env["SCRIPT_NAME"] = _cgi._path;
    _cgi._env["SCRIPT_FILENAME"] = _cgi._path;
    _cgi._env["CONTENT_LENGTH"] = std::to_string(_body.size());
    _cgi._env["CONTENT_TYPE"] = getHeaderField("Content-Type");
    _cgi._env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _cgi._env["PATH_INFO"] = getPath();
    _cgi._env["PATH_TRANSLATED"] = getPath();
    _cgi._env["QUERY_STRING"] = getQueryString();
    _cgi._env["HTTP_COOKIE"] = getHeaderField("Cookie");
    _cgi._env["REMOTE_ADDR"] = getHost().substr(0, getHost().find(":"));
    _cgi._env["SERVER_NAME"] = getServerName();
    _cgi._env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _cgi._env["SERVER_PORT"] = std::to_string(getPort());
    _cgi._env["HTTP_USER_AGENT"] = getHeaderField("USER_AGENT");
    _cgi._env["REDIRECT_STATUS"] = "200";
    _cgi._env["REQUEST_METHOD"] = getMethod();
    _cgi._env["SERVER_SOFTWARE"] = "Webserv/2.0";
}



char **setEnvExecve(t_cgi &cgi)
{
	char **env = new char*[cgi._env.size() + 1];
	int				i = 0;

	std::map<std::string, std::string>::const_iterator it = cgi._env.begin();
	
	for (; it != cgi._env.end(); ++it)
	{
		std::string envEntry = it->first + "=" + it->second + "\0";
		env[i] = new char[envEntry.size() + 1];
		env[i] = std::strcpy(env[i], envEntry.c_str());
		i++;
	}
	env[i] = NULL;
	return (env);
}

/**
 * executeCGI
 * This function handles the execution of the CGI script in a child process.
 * Steps involved:
 * 1. Sets up environment variables for `execve`.
 * 2. Creates two pipes for communication between the parent and child processes:
 * 3. Forks a new process:
 *     - **Child Process:**
 *         c. Executes the CGI script using `execve`.
 *         d. Exits with status 502 if `execve` fails.
 *     - **Parent Process:**
 *         b. Writes the request body to the write end of `_fdin`.
 *         d. Calls `readSafeTimeout` to read the CGI output from the read end of `_fdout`.
 *         e. Waits for the CGI process to complete using `waitpid`.
 * 4. Handles the exit status of the CGI process and sets the HTTP response status accordingly.
 */
void	Request::executeCGI()
{
	char **env = setEnvExecve(_cgi);
	for (int i = 0; env[i]; i++)
		std::cerr << BG_GREEN "env is : " << env[i] << RESET << std::endl;
	if (!isExecutable(_cgi._execPath))
	{
		this->_status = 500;
		return ;
	}
	if (pipe(_cgi._fdin) == -1 || pipe(_cgi._fdout) == -1)
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
		dup2(_cgi._fdin[0], STDIN_FILENO);
		dup2(_cgi._fdout[1], STDOUT_FILENO);
		close(_cgi._fdin[1]);
		close(_cgi._fdout[0]);
		
		const char* args[3] = {_cgi._execPath.c_str(), _cgi._path.c_str(), NULL};
		execve(_cgi._execPath.c_str(),  const_cast<char* const*>(args), env);
		this->_status = 502;
		std::cerr << BG_RED "execve failed" RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	close(_cgi._fdin[0]);
	close(_cgi._fdout[1]);
	close(_cgi._fdin[1]);
	for (int i = 0; env[i]; i++)
		delete env[i];
	delete env;

	if (pid > 0)
	{
		_cgi._pid = pid;
		_cgi._start = std::time(NULL);
		_cgi._onCGI = true;
	}
	else
	{
		_cgi._onCGI = false;
		close(_cgi._fdout[0]);
	}
}


bool isExecutable(const std::string& path) 
{
	struct stat info;
    return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IXUSR);
}

void Request::checkCGISHeader()
{
	if (_status != 200)
		return;
	this->_respbody = _cgi._respbody;
	if (_respbody.empty() || _respbody.find("Content-Type:") == std::string::npos)
	{	
		_status = 502;
		_respbody.clear();
		return;
	}
	size_t headerEnd = _respbody.find("\r\n\r\n");
	int delimSize = 4;
	if (headerEnd == std::string::npos)
	{
		headerEnd = _respbody.find("\n\n");
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
	if (header.find("Content-Length:") == std::string::npos)
	{
		std::cerr << BG_RED "Content-Length is not present" RESET << std::endl;
		int	len = body.length();
		std::ostringstream oss;
		oss << "Content-Length: " << len << "\r\n";
		_respbody = oss.str() + _respbody;
		this->_status = 200;
		return;
	}
	this->_status = 200;
}