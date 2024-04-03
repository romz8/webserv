/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:51:53 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/03 21:37:08 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


/*
Initializes a Response object using the provided Header object.
Sets the HTTP status, version, and the path to the asset to be returned 
in the response body.
TBD : INCLUDE A AUTO ERROR /FETCH ERROR PAGE logic if code not 200
*/
Response::Response(Header& head)
{
	this->_status = head.getStatus();
	this->_method = head.getMethod();
	this->_version = "HTTP/1.1";
	if (this->_status == 404)
		this->_assetPath = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/error_pages/404.html";
	else if (this->_status == 401)
		this->_assetPath = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/error_pages/401.html";
	else
		this->_assetPath = head.getParsePath();
	std::cout << "Respons obj built with : " << this->_status << " and " << this->_assetPath << std::endl;
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
	if(this->_method == "GET")
		this->excecuteGetResponse();
	else
		return ; //later on we will do the POST, DELETE and UNknown case
	
	this->_reponse = this->_headerResponse + this->_body;
	std::cout << BLUE "FINAL CHECK ON RESP : " << this->_reponse << RESET << std::endl;
}

void	Response::excecuteGetResponse()
{
	setStatusLine(this->_status);
	
	//to check - delete later on 
	std::cout << "Status-Line is : " << this->_statusLine << std::endl;
	std::cout << "file path is : " << this->_assetPath << std::endl;

	switch(this->_status)
	{
		case(200):
			handle200();
			break;
		case(301):
			handle301();
			break;
		default:
			handleError();
			break;
	}
	//to check - delete later on 
	std::cout << "Header is  : " << this->_headerResponse << std::endl;
	std::cout << "Body is  : " << this->_body << std::endl;
}

void Response::setStatusLine(int sCode) 
{
    std::ostringstream StatusLine;
	
	switch (sCode) {
        case 200:
            this->_statusMsg = "OK";
            break;
        case 301:
            this->_statusMsg = "Moved Permanently";
            break;
        case 405:
            this->_statusMsg = "Method Not Allowed";
            break;
        case 404:
            this->_statusMsg = "Not Found";
            break;
        case 403:
            this->_statusMsg = "Forbidden";
            break;
        case 500:
            this->_statusMsg = "Internal Server Error";
            break;
        case 505:
            this->_statusMsg = "HTTP Version not supported";
            break;
        default:
            // Optionally handle unexpected status codes
            // this->_statusMsg = "Internal Server Error";
			// this->_statusMsg = "Internal Server Error";
			this->_statusMsg = "Unknown Status"; //for debuggin, otherwise will be 500
            break;
    }
	
	StatusLine << this->_version << " " << this->_status << " " << this->_statusMsg << "\r\n";
	this->_statusLine = StatusLine.str();
}

std::string Response::getResponse() const
{
	return(this->_reponse);
}

void 	Response::handle200()
{
	std::ostringstream head;
	
	this->setBody();
	this->_content_type = "text/html; charset=UTF-8";
	head << "Content-Length: " << this->_content_length << "\r\n";
	head << "Content-Type: " << this->_content_type << "\r\n";
	head << "\r\n";
	this->_headerContent = head.str();
	this->_headerResponse = this->_statusLine.append(this->_headerContent);
}

void 	Response::handle301()
{
	this->_headers["Location"] = this->_assetPath;
	
	this->_headerContent = "Location: " + _headers["Location"] + " \r\n";  // this will be changed to auto converter of header later on;
	this->_headerResponse = this->_statusLine.append(this->_headerContent);
}

void 	Response::handleError()
{
	std::ostringstream head;
	
	this->setBody();
	this->_content_type = "text/html; charset=UTF-8";
	head << "Content-Length: " << this->_content_length << "\r\n";
	head << "Content-Type: " << this->_content_type << "\r\n";
	head << "\r\n";
	this->_headerContent = head.str();
	this->_headerResponse = this->_statusLine.append(this->_headerContent);
}
