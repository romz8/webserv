/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:55:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/29 14:46:16 by rjobert          ###   ########.fr       */
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
			throw std::runtime_error("Error parsing header");
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