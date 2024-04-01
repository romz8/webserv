/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:55:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/01 17:55:03 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

Header::Header(const std::string& rawHeader)
{
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
MISSING ERROR HADNLING (on ';' for example)
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
//quick and dirty parsing to test and compare
// void Header::parseHeader(const std::string& head)
// {
// 	std::string::size_type start = 0;
// 	std::string::size_type end = head.find("\r\n", start);
// 	std::string::size_type pos = head.find(" ", start);
// 	if (pos == std::string::npos)
// 		throw std::runtime_error("Error parsing header");
// 	this->_method = head.substr(start, pos - start);
// 	start = pos + 1;
// 	pos = head.find(" ", start);
// 	if (pos == std::string::npos)
// 		throw std::runtime_error("Error parsing header");
// 	this->_path = head.substr(start, pos - start);
// 	start = pos + 1;
// 	pos = head.find("\r\n", start);
// 	if (pos == std::string::npos)
// 		throw std::runtime_error("Error parsing header");
// 	this->_version = head.substr(start, pos - start);
// 	start = pos + 2;
	// while (start < head.size())
	// {
	// 	pos = head.find("\r\n", start);
	// 	if (pos == std::string::npos)
	// 		throw std::runtime_error("Error parsing header");
	// 	this->_headers.push_back(head.substr(start, pos - start));
	// 	start = pos + 2;
	// }
//}

void Header::printHeader() const
{
	std::cout << "Method : " << this->_method << std::endl;
	std::cout << "Path : " << this->_path << std::endl;
	std::cout << "Version : " << this->_version << std::endl;
	std::cout << "Headers : " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		std::cout << it->first << " :" << it->second << std::endl;
	}
}

void	Header::buildRequest()
{
	if (!isValidMethod())
		this->_status = 405;
	if (!isValidVersion())
		this->_status = 505;
	if (!isValidPath())
		this->_status = 404;
	std::cout << "Request is valid : " << this->_method << " " << this->_path << " " << this->_version << std::endl;
	this->_status = 200;
}

bool Header::isValidMethod() const
{
	if (this->_method == "GET" || this->_method == "POST" || this->_method == "DELETE")
		return (true);
	return (false);
}

bool Header::isValidPath() 
{
	std::string testpath = "/Users/rjobert/Desktop/42_cursus/webserv/proto"; //to be cahnge for dynamic root and env var
	if (this->_path.empty())
		return (false);
	if (this->_path[0] != '/')
		return (false);
	if (this->_path[this->_path.size() - 1] == '/')
	{
		this->_isDirectory = true;
		this->_parsePath = testpath.append(this->_path.substr(0, this->_path.size() - 1));
		if (!(this->isDirectory()))
			return (false);
	}
	else
	{
		this->_isDirectory = false;
		this->_parsePath = testpath.append(this->_path);
		if (!(this->fileExists()))
			return (false);
	}
	return (true);
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

bool Header::fileExists() const
{
	struct stat buffer;
	std::cout << "Path in file exist: " << this->_parsePath << std::endl;
	return (stat(this->_parsePath.c_str(), &buffer) == 0);
}

bool Header::isDirectory() const
{
	struct stat buffer;
	if (stat(this->_parsePath.c_str(), &buffer) == 0)
		return (S_ISDIR(buffer.st_mode));
	return (false);
}

int getStatus() const
{
	return (this->_status);
}