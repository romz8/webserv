/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:34 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/22 15:43:28 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//HARDCODED A LOT OF ENV VARIABLES, NEED TO BE CHANGED -> with Merger on Config Parsing
CGI::CGI(Request &req, std::string execpath) : _path(req.getParsePath()), _body(req.getBody()), _execPath(execpath)
{
	_status = 0;
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
	_env["REMOTE_ADDR"] = req.getHost().substr(0, req.getHost().find(":"));
	_env["SERVER_NAME"] = req.getServerName();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_PORT"] = std::to_string(req.getPort());
	_env["HTTP_USER_AGENT"] = req.getHeaderField("USER_AGENT");
	_env["REDIRECT_STATUS"] = "200";
	_env["REQUEST_METHOD"] = req.getMethod();
	_env["SERVER_SOFTWARE"] = "Webserv/2.0";

	std::cerr << BG_RED << "CGI ENV PATH " << this->_env["SCRIPT_NAME"] << RESET << std::endl;
	//_path = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/cgi-bin/hellocgi.sh";
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

	_pid = pid;
	_start = std::time(NULL);

	std::string newbody = readSafeTimeout();
	close(_fdout[0]);
	for (int i = 0; env[i]; i++)
		delete env[i];
	delete env;
	
	if (_status == 508)
		return;
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		_respbody = newbody;
		this->_status = 200;
	}
	else
	{
		this->_status = 502;
		std::cout << BG_RED "CGI script failed" RESET << std::endl;
	}
}
	

int CGI::getStatus(void)
{
	return (_status);
}

std::string CGI::getBody(void)
{
	return (_respbody);
}

/**
 * checkCGI
 * This function validates the CGI response to ensure it is correctly formatted.
 * 
 * Steps involved:
 * 1. Checks if the status is not 200 (OK) and returns early if not.
 * 2. Checks if the response body is empty or lacks a `Content-Type` header.
 * 3. Searches for the end of the HTTP headers using double newlines.
 * 4. Extracts and separates the headers and body:
 * 5. Checks if the `Content-Length` header is present and add-it if not
 * 6. Returns a correctly formatted CGI response.
 */
void CGI::checkCGI()
{
	if (_status != 200)
		return; 
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
		return;
	}
}
/**
 * readSafeTimeout
 * This function reads data from the CGI process output pipe within a specified timeout period.
 * 
 * Steps involved:
 * 1. Initializes a buffer to read data from the CGI output.
 * 2. Enters a loop that continues until the timeout is exceeded or all data is read:
 * 3. If the timeout is exceeded before all data is read, kill CGI process and set 508 status
 * 4. Returns the collected CGI output as a string.
 *
 * @return String containing the CGI output
 */
std::string	CGI::readSafeTimeout(void)
{
	bool readComplete = false;
	std::string buffBody;
	char buffer[1024];

	buffBody.clear();
	
	while (std::time(NULL) - _start < _TimeoutSec)
	{
		int byteRead;
		byteRead = read(_fdout[0], buffer, sizeof(buffer) - 1);
		if (byteRead > 0)
		{
			buffer[byteRead] = '\0';
			buffBody += std::string(buffer, byteRead);
		}
		else
		{
			readComplete = true;
			break;
		}
	}
	if (!readComplete)
	{
		kill(_pid, SIGKILL);
		_status = 508;
	}
	return (buffBody);
}

const pid_t CGI::getPid(void) const
{
	return (_pid);
}

bool isExecutable(const std::string& path) 
{
	struct stat info;
    return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IXUSR);
}