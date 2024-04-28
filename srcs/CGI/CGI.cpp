/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:34 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/28 18:39:03 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(Request &req) : _path(req.getPath()), _body(req.getBody())
{
	_env["AUTH_TYPE"] = "";
	this->_env["SCRIPT_NAME"] = _path;
	this->_env["SCRIPT_FILEAME"] = _path;
	_env["CONTENT_LENGTH"] = std::to_string(_body.size());
	_env["CONTENT_TYPE"] = req.getHeaderField("Content-Type");
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["PATH_INFO"] = req.getPath();
	_env["PATH_TRANSLATED"] = req.getPath();
	_env["QUERY_STRING"] = req.getHeaderField("Query"); //to be implemented
	_env["REMOTE_ADDR"] = "0.0.0.0";
	_env["SERVER_NAME"] = "webserv"; // from config
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_PORT"] = _serverConfig.port; //FROM CONFIG
	_env["HTTP_USER_AGENT"] = req.getHeaderField("USER_AGENT");
	_env["REDIRECT_STATUS"] = "200";
	_env["REQUEST_METHOD"] = req.getMethod();
	_env["SERVER_SOFTWARE"] = "Webserv/2.0";
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
	char		*argv[3];
	char			*env[10];
	int				i = 0;

	argv[0] = const_cast<char *>(_path.c_str());
	argv[1] = NULL;
	std::map<std::string, std::string>::const_iterator it = _env.begin();
	
	for (; it != _env.end(); ++it)
	{
		std::string envEntry = it->first + "=" + it->second + '\0';
		env[i] = new char[envEntry.size() + 1];
		env[i] = const_cast<char *>(envEntry.c_str());
	}
	env[i] = NULL;
	//execve(argv[0], argv, env);
}
void	executeCGI()
{
	
}
