/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:55:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/03 21:43:20 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

Header::Header(const std::string& rawHeader)
{
	initRequest();
	parseHeader(rawHeader);
}

Header::~Header(){}

Header::Header(const Header& src)
{
	*this = src;
}

Header& Header::operator=(const Header& src)
{
	if (this != &src)
	{
		this->_method = src._method;
		this->_path = src._path;
		this->_version = src._version;
		this->_headers = src._headers;
	}
	return (*this);
}

/*

first we parse the start line (containing the method, path and version)
then we parse the headers as key value pairs inside a map
we implement a trimming for key, value for insrting in the map
*/
void Header::parseHeader(const std::string& head)
{
	std::istringstream stream(head);
	std::string line;
	std::getline(stream, line);
	std::istringstream lineStream(line);
	lineStream >> this->_method >> this->_path >> this->_version;
	line.clear();
	while (std::getline(stream, line) && !(line == "\r"))
	{
		if (line.empty())
			break;
		std::string::size_type pos = line.find(":");
		if (pos == std::string::npos)
			throw std::runtime_error("Error parsing header : no colon (:)found in" + line);
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		key.substr(key.find_first_not_of(" \t\n\r\f\v"), key.find_last_not_of(" \t\n\r\f\v") - key.find_first_not_of(" \t\n\r\f\v") + 1);
		value.substr(value.find_first_not_of(" \t\n\r\f\v"), value.find_last_not_of(" \t\n\r\f\v") - value.find_first_not_of(" \t\n\r\f\v") + 1);
		this->_headers[key] = value;
		line.clear();
	}
}

void Header::printHeader() const
{
	std::cout << YELLOW "Method : " << this->_method << std::endl;
	std::cout << "status is : " << this->_status << std::endl;
	std::cout << "Path : " << this->_path << std::endl;
	std::cout << "Version : " << this->_version << std::endl;
	std::cout << "Headers : " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		std::cout << it->first << " :" << it->second << std::endl;
	}
	std::cout << RESET << std::endl;
}

void	Header::initRequest()
{
	this->_isDirectory = false;
	this->_isDirNorm = false;
	this->_status = 0;
	this->_method.clear();
	this->_path.clear();
	this->_version.clear();
	this->_parsePath.clear();
	this->_respBody.clear();
}
/*
Analyzes the HTTP request details stored in the Header object to determine the appropriate
HTTP response status code. This function sequentially checks various aspects of the request,
including the HTTP method, version, and the requested path's validity and accessibility.

Process Flow:
1. Checks if the request uses a valid HTTP method (e.g., GET, POST). If not, sets the
   status code to 405 (Method Not Allowed) and exits.
2. Validates the HTTP version specified in the request. If it's not supported, sets the
   status code to 505 (HTTP Version Not Supported) and exits.
3. Examines the requested path to ensure it exists and is correctly formatted. If the path
   is invalid, sets the status code to 404 (Not Found) and exits.
4. For requests targeting a directory, it checks if the path needs normalization (i.e., adding
   a trailing slash). If so, it normalizes the path and sets the status code to 301 (Moved
   Permanently) to redirect the client to the normalized path.
5. Finally, it checks if the requested resource is accessible (e.g., read permissions are
   available). If access is granted, sets the status code to 200 (OK); otherwise, sets it to
   403 (Forbidden).
*/
void	Header::buildRequest()
{
	if (!isValidMethod())
	{
		this->_status = 405;
		return ;
	}
	if (!isValidVersion())
	{
		this->_status = 505;
		return ;
	}
	if (!isValidPath())
	{
		this->_status = 404;
		return ;
	}
	if (this->_isDirectory)
	{
		this->normalizeDirPath();
		if (this->_isDirNorm)
		{
			this->_status = 301;
			return;
		}	
	}
	this->_status = hasReadAccess() ? 200 : 403;
	std::cout << YELLOW "Request status-Line is : " << this->_method << " " << this->_path << " " << this->_version << RESET << std::endl;
}

bool Header::isValidMethod() const
{
	if (this->_method == "GET" || this->_method == "POST" || this->_method == "DELETE")
		return (true);
	return (false);
}

/*
verify that path is ok at first (not empty, starting with /)
then check if the path is either a file or a dir (with stat())
if neither -> raiseError by returning null, otherwise update Response State
*/
bool Header::isValidPath() 
{
	std::string testpath = "/Users/rjobert/Desktop/42_cursus/webserv/proto"; //to be cahnge for dynamic root and env var
	struct stat path_stat;
	
	if (this->_path.empty())
		return (false);
	if (this->_path[0] != '/')
		return (false);
		
	this->_parsePath = testpath.append(this->_path);
	
	if (stat(this->_parsePath.c_str(), &path_stat) == -1)
		return(false);
	if(S_ISDIR(path_stat.st_mode))
		this->_isDirectory = true;
	else if(S_ISREG(path_stat.st_mode))
		this->_isDirectory = false;
	else 
		return (false);
	return (true);
}

/*
if a path leads to a valid (existing) dir, we check it does end by '/', 
if not we add the / and redirect it to the correct path/dir/ address by
returning a 301 with path to the browser
this is necessary as stat doesn't check this but http requires this management
*/
void Header::normalizeDirPath()
{
	std::string path = this->_parsePath;
	if (path[path.size() - 1] == '/')
		return ;
	this->_isDirNorm = true;
	this->_parsePath = this->_path.append("/");
	return ;
}



bool Header::isValidVersion() const
{
	if (this->_version == "HTTP/1.1")
		return (true);
	return (false);
}

std::string Header::getMethod() const
{
	return (this->_method);
}

std::string Header::getPath() const
{
	return (this->_path);
}

std::string Header::getParsePath() const
{
	return (this->_parsePath);
}

// int Header::fileExists() const
// {
// 	struct stat buffer;
// 	std::cout << "Check that Path in file exist: " << this->_parsePath << std::endl;
// 	if(stat(this->_parsePath.c_str(), &buffer) == -1)
// 	{
// 		std::cout <<"FILE NOT EXIST!!\n";
// 		return (-1);
// 	}
// 	std::cout <<"FILE EXIST ??? \n";
// 	return(0);
// }

// bool Header::isDirectory() const
// {
// 	struct stat buffer;
// 	if (stat(this->_parsePath.c_str(), &buffer) == 0)
// 		return (S_ISDIR(buffer.st_mode));
// 	return (false);
// }

int Header::getStatus() const
{
	return (this->_status);
}

// R_OK tests for readability only
bool Header::hasReadAccess() const
{	
    if (access(this->_parsePath.c_str(), R_OK) == 0)
		return(true);
	return (false);
}