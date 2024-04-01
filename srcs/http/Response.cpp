/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:51:53 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/01 18:00:26 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

Response::Response(Header& head)
{
	this->_status = head->getStatus();
	this->_version = head.getVersion();
	this->_headers = head.getHeaders();
	this->_body = "";
}

Response::~Response(){}

Response::Response(const Response& src)
{
	*this = src;
}

Response& Response::operator=(const Response& src)
{
	if (this != &src)
	{
		this->_status = src._status;
		this->_version = src._version;
		this->_headers = src._headers;
		this->_body = src._body;
	}
	return (*this);
}

// void Response::buildResponse()
// {
// 	std::ostringstream response;
// 	response << this->_version << " " << this->_status << "\r\n";
// 	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
// 		response << it->first << ": " << it->second << "\r\n";
// 	response << "\r\n";
// 	response << this->_body;
// 	std::cout << response.str() << std::endl;
// }