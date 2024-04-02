/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:51:53 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/02 16:53:31 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


/*
Initializes a Response object using the provided Header object.
Sets the HTTP status, version, and the path to the asset to be returned 
in the response body.
*/
Response::Response(Header& head)
{
	this->_status = head.getStatus();
	this->_version = "HTTP/1.1";
	this->_assetPath = head.getParsePath();
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


/*
Opens and reads the content of the asset file specified by _assetPath.
Sets the _body member to the content of the file, updating _content_length 
accordingly.
Handles the error case where the file cannot be opened by setting the
status to 500.

*/
void	Response::setBody()
{
	std::ifstream htmlBody(this->_assetPath, std::ios::in);
	if (!htmlBody.is_open())
	{
		this->_status = 500;
		return ;
	}
	std::stringstream buff;
	buff << htmlBody.rdbuf();
	this->_body = buff.str();
	this->_content_length = this->_body.size();
	return ;
}

/*
Constructs the full HTTP response message.
Calls setBody to load the response body from the specified asset path.
Sets the reason phrase based on the HTTP status code.
Constructs the response headers, including Content-Length and Content-Type.
Assembles the full response string, ready to be sent back to the client.
void Response::setReasonPhrase(int sCode)
Sets the reason phrase (_statusMsg) based on the provided status code (sCode).
Handles several common status codes (200, 405, 505, 404) with default messages.
std::string Response::getResponse() const
Returns the full HTTP response message as a string.
*/
//additional check on the response to allow 404 (impossible to open thought the pathfile was fine)
//MISSING TIME AND SERVER_NAME
void Response::buildResponse()
{
	std::ostringstream response;
	this->setBody();
	setReasonPhrase(this->_status);
	response << this->_version << " " << this->_status << " " << this->_statusMsg << "\r\n";
	std::cout << "Status-Line is : " << response.str() << std::endl;
	
	this->_content_type = "text/html; charset=UTF-8";
	response << "Content-Length: " << this->_content_length << "\r\n";
	response << "Content-Type: " << this->_content_type << "\r\n";
	response << "\r\n";
	response << this->_body;
	this->_reponse = response.str();
	std::cout << this->_reponse << std::endl;
}


void	Response::setReasonPhrase(int sCode)
{
	// potentially refacto for a switch statement
	if (sCode == 200)
		this->_statusMsg = "OK";
	else if (sCode == 405)
		this->_statusMsg = "Method Not Allowed";
	else if (sCode == 505)
		this->_statusMsg = "HTTP Version not supported";
	else if (sCode == 404)
		this->_statusMsg = "Not Found";		
}

std::string Response::getResponse() const
{
	return(this->_reponse);
}