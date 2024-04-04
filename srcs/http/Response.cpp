/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:51:47 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/04 22:07:52 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

const std::string Response::_root = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html"; // Definition

/*
Initializes a Response object using the provided Header object.
Sets the HTTP status, version, and the path to the asset to be returned 
in the response body.
TBD : INCLUDE A AUTO ERROR /FETCH ERROR PAGE logic if code not 200
*/
Response::Response(Header& head) : _status(head.getStatus()), _method(head.getMethod()), \
	_version("HTTP/1.1"), _statusMsgs(initStatusMaps()), _errPages(initErrMaps()), \
	_mimeTypes(initMimeMaps()), _content_len("0"), _headerResponse(""), _body(""), \
	_response(""), _assetPath(""), _extension(head.getExtension())
{
	if (_errPages.find(this->_status) != _errPages.end())
		this->_assetPath = _root + _errPages[this->_status];
	else if (head.getParsePath().empty())
		this->_assetPath = _root + _errPages[404];
	else
		this->_assetPath = head.getParsePath();
	std::cout << RED "Respons obj built with : " << this->_status << " and " << this->_assetPath << std::endl;
	std::cout << "extension is " << this->_extension << std::endl;
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
	finalizeHeader();
	//std::cout << BLUE "FINAL CHECK ON RESP : " << this->_response << RESET << std::endl;
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
	std::cout << "Header is  : " << this->_headerResponse << std::endl;
	//std::cout << "Body is  : " << this->_body << std::endl;
}

void Response::setStatusLine(int sCode) 
{
    std::ostringstream StatusLine;
	
	this->_statusMsg = getStatusMessage(sCode);
	StatusLine << this->_version << " " << this->_status << " " << this->_statusMsg << "\r\n";
	this->_statusLine = StatusLine.str();
}

std::string Response::getResponse() const
{
	return(this->_response);
}

void	Response::addHeader()
{
	if (this->_status == 301)
		_headers["Location"] = this->_assetPath;
	else
	{	
		_headers["Content-Type"] = getMimeType(this->_extension);
		_headers["Content-Length"] = _content_len;
	}
}

void 	Response::handle200()
{
	std::ostringstream head;
	
	this->setBody();
	addHeader();
}

void 	Response::handle301()
{
	addHeader();
}

void 	Response::handleError()
{
	std::ostringstream head;
	
	this->setBody();
	addHeader();
}

std::string Response::assembHeader()
{
	std::ostringstream headerStream;
	std::map<std::string, std::string>::const_iterator it = this->_headers.begin();
	for (; it != _headers.end(); ++it)
	{
		std::cout << RED "HEADER IS : " << it->first << " : " << it->second << RESET << std::endl;
		headerStream << it->first << ": " << it->second << "\r\n";
	}
	headerStream << "\r\n";
	return (headerStream.str());
}

void Response::finalizeHeader()
{
	std::string headContent = assembHeader();
	this->_response = this->_statusLine + headContent;
	this->_response.append(_body);
	
	std::cout << "Header is  : " << this->_headerResponse << std::endl;
	// std::cout << "Body is  : " << this->_body << std::endl;
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
	std::ifstream fBody(this->_assetPath, std::ios::binary | std::ios::ate);
	if (!fBody.is_open())
	{
		this->_status = 500;
		return ;
	}
	std::streampos fileSize = fBody.tellg(); 
	fBody.seekg(0, std::ios::beg);
	
	 std::string buff(fileSize, '\0');
	fBody.read(&buff[0], fileSize);
	this->_body = buff;
	
	// Convert std::streampos to std::string
    std::ostringstream oss;
    oss << fileSize;
    this->_content_len = oss.str();
	
	fBody.close();
	return ;
}

// Initializing MIME Types
 std::map<std::string, std::string> Response::initMimeMaps() 
{
    std::map<std::string, std::string> m;
	
	m[".html"] = "text/html";
    m[".css"] = "text/css";
    m[".jpg"] = "image/jpeg";
    m[".png"] = "image/png";
    m[".gif"] = "image/gif";
    m[".js"] = "application/javascript";
    m[".pdf"] = "application/pdf";
    m[".json"] = "application/json";
    m[".mp4"] = "video/mp4";
    m[".mp3"] = "audio/mpeg";
    m[".txt"] = "text/plain";
    m[".csv"] = "text/csv";
	m[".ico"] = "image/x-icon";

	return m;
}

// Initializing Status Messages
std::map<int, std::string>	Response::initStatusMaps()
{
    std::map<int, std::string> s;
	
	s[200] = "OK";
    s[301] = "Moved Permanently";
    s[400] = "Bad Request";
	s[403] = "Forbidden";
    s[404] = "Not Found";
    s[405] = "Method Not Allowed";
    s[500] = "Internal Server Error";
    s[505] = "HTTP Version not supported";

	return s;
}

// Initializing Error Pages
std::map<int, std::string> Response::initErrMaps()
{
    std::map<int, std::string> e;
	e[400] = "/error_pages/400.html";
	e[403] = "/error_pages/403.html";
    e[404] = "/error_pages/404.html";
    e[500] = "/error_pages/500.html";
    e[405] = "/error_pages/405.html";
    e[505] = "/error_pages/505.html";
	
	return e;
}

// Get the MIME type based on file extension
std::string Response::getMimeType(const std::string& extension) const 
{
    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(extension);
    if (it != _mimeTypes.end())
        return it->second;
    else
        return "text/plain"; // Default MIME type if not found
}

// Get the status message based on status code
std::string Response::getStatusMessage(int statusCode) const 
{
    std::map<int, std::string>::const_iterator it = _statusMsgs.find(statusCode);
    if (it != _statusMsgs.end()) 
        return it->second;
	else
        return "Unknown Status"; // Default message if not found
}
