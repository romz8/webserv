/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:51:47 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/22 14:47:51 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

/* Response Constructor
   Initializes a Response object using the provided Request object. Sets the HTTP status,
   version, and the path to the asset to be returned in the response body. Automatically selects
   an error page if the HTTP status code indicates a client or server error.
   Parameters:
   - head: A reference to the Request object based on which the Response is constructed.
*/
Response::Response(Request& head) : _status(head.getStatus()), _method(head.getMethod()), \
	_version("HTTP/1.1"), _statusMsgs(initStatusMaps()), _mimeTypes(initMimeMaps()), \
	_content_len("0"), _headerResponse(""), _body(head.getrespBody()), _response(""), _assetPath(""), \
	_extension(".html"), _location(head.getLocation()), _fromCgi(head.execCgi())
{
	if (this->_status >= 400)
		this->_assetPath = _location.getRootDir() + getErrorPage(this->_status);
	else if (head.getParsePath().empty())
		this->_assetPath = _location.getRootDir() + getErrorPage(404);
	else
		this->_assetPath = head.getParsePath();
	// std::cout << "parsed Path is : "<< head.getParsePath() << std::endl;
	std::cout << RED "Respons obj built with : " << this->_status << " and " << this->_assetPath << std::endl;
	// std::cout << "extension is " << this->_extension << std::endl;
}

Response::~Response(){}

Response::Response(const Response& src)
{
	*this = src;
}

/*TBD : PUT ALL THE MEMBER ATTRIBUTE*/
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
 * ------------------------
 * Constructs the complete HTTP response message based on the state of the Response object.
 * This function "orchestrates" the assembly of the status line, headers, and body into a
 * single coherent response string. It ensures that the response adheres to the HTTP
 * standard by including appropriate status lines, headers, and content.
 * Workflow:
 *   1. Sets the status line using the current status code.
 *   2. Adds all necessary HTTP headers by calling `addHeaders`.
 *   3. If the status is not one of the types that should have no body (like 204 No Content),
 *      it calls `setBody` to prepare the response content.
 *   4. Finally, it calls `finalizeResponse` to restructure all parts into the full HTTP response.
 * The resulting full HTTP response is stored in `_response`, ready to be sent to the client.
*/
//MISSING TIME AND SERVER_NAME
void Response::buildResponse()
{
	setStatusLine(this->_status);
	//std::cout << BLUE "Status-Line is : " RESET << this->_statusLine << std::endl;
	//std::cout << "file path is : " << this->_assetPath << std::endl;
	if (!(this->_status == 301 || this->_status == 201 || this->_status == 204))
		this->setBody();
	addHeaders();
	finalizeResponse();
}

/* setStatusLine
   Sets up the HTTP status line using the current status code.
   Parameters:
   - sCode: The HTTP status code to set in the status line.
*/
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

/* addHeaders
   Adds necessary HTTP headers based on the current state of the Response object.
   Sets headers such as 'Location' and 'Content-Length' as appropriate.
*/
void	Response::addHeaders()
{
	if (_fromCgi)
		return;
	if (this->_status == 301)
	{
		_headers["Location"] = this->_assetPath;
		_headers["content-length"] = "0";
		//_headers["Connection"] = "close";
	}
	else if (this->_status == 204)
		_headers.clear();
	else if (this->_status == 201)
	{
		_headers["Location"] = this->_assetPath; //migth be useless - to test
		_headers["content-length"] = "0";
	}
	else
	{	
		_headers["Content-Type"] = getMimeType(this->_extension);
		_headers["Content-Length"] = _content_len;
	}
}

/* assembHeaders
   Constructs the headers part of the HTTP response from the headers map.
   Returns: A string formatted as HTTP headers.
*/
std::string Response::assembHeaders()
{
	std::ostringstream RequestStream;
	std::map<std::string, std::string>::const_iterator it = this->_headers.begin();
	for (; it != _headers.end(); ++it)
		RequestStream << it->first << ": " << it->second << "\r\n";
	RequestStream << "\r\n";
	return (RequestStream.str());
}


void Response::finalizeResponse()
{
	if (_fromCgi && !(this->_status >= 400))
	{
		this->_response = this->_statusLine + this->_body;
		return;
	}
	std::string headContent = assembHeaders();
	this->_response = this->_statusLine + headContent;
	this->_response.append(_body);
	
}

/* setBody
   Opens and reads the content of the asset file specified by the asset path.
   Sets the _body member to the content of the file, updating the content length accordingly.
   Handles the error case where the file cannot be opened by setting the status to 500.
*/
void	Response::setBody()
{
	try
	{
		if (_fromCgi && !(this->_status >= 400))
			return;
		if (!(this->_body.empty()))
		{
			this->_extension = ".html";
			this->_content_len = std::to_string(this->_body.size());
		}
		else
		{
			std::cout << "this case with asset path : " << this->_assetPath << std::endl;
			this->_body = readWebFile(this->_assetPath);
			this->_extension = parseExtension(this->_assetPath, this->_extension);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << "Error settign body response ";
		std::cerr << e.what() << std::endl;
		this->_status = 500;
		this->_body.clear();
        this->_content_len = "0";
	}
}

/* readWebFile
* Opens and reads the contents of a file specified by `assetPath`.
 * This function is designed to handle binary file reading, for serving non-text 
 * files like images or PDFs. It opens the file in binary mode then do byte measurement 
 * with ptrs (seekg, tellg) and reads all contents into a buffer, 
 * which is then returned as a string.
   Parameters:
   - assetPath: The path to the file to read.
   Returns: A string containing the contents of the file.
*/
std::string Response::readWebFile(const std::string& assetPath)
{
	std::ifstream file(assetPath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("Impossible opening the content to serve");
	
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	
	std::vector<char> buffer(fileSize);
	if (!file.read(&buffer[0], fileSize))
		throw std::runtime_error("error reading content to serve");
	file.close();
	std::ostringstream oss;
    oss << fileSize;
    this->_content_len = oss.str();
	return (std::string(buffer.begin(), buffer.end()));
}

/* initMimeMaps
   Initializes the map that associates file extensions with MIME types.
   Returns: A map where keys are string extensions and values are the corresponding MIME types.
*/
 std::map<std::string, std::string> Response::initMimeMaps() 
{
    std::map<std::string, std::string> m;
	
	m[".html"] = "text/html";
    m[".css"] = "text/css";
    m[".jpeg"] = "image/jpeg";
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


/* initStatusMaps
   Initializes the map associating HTTP status codes with their textual descriptions.
   Returns: A map of integer HTTP status codes to string messages.
*/
std::map<int, std::string>	Response::initStatusMaps()
{
    std::map<int, std::string> s;
	
	s[200] = "OK";
	s[201] = "Created";
	s[204] = "No Content";
    s[301] = "Moved Permanently";
    s[400] = "Bad Request";
	s[403] = "Forbidden";
    s[404] = "Not Found";
	s[408] = "Request Timeout";
	s[409] = "Conflict";
	s[413] = "Content Too Large";
    s[405] = "Method Not Allowed";
    s[500] = "Internal Server Error";
	s[502] = "Bad Gateway";
    s[505] = "HTTP Version not supported";

	return s;
}

/* getMimeType
   Retrieves the MIME type associated with a file extension.
   Parameters:
   - extension: The file extension to look up.
   Returns: The MIME type as a string, defaults to 'text/plain' if not found.
*/
std::string Response::getMimeType(const std::string& extension) const 
{
    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(extension);
    if (it != _mimeTypes.end())
        return it->second;
    else
        return "text/plain"; // Default MIME type if not found
}

/* getStatusMessage
   Retrieves the message associated with an HTTP status code.
   Parameters:
   - statusCode: The HTTP status code to look up.
   Returns: The associated message as a string, defaults to 'Unknown Status' if not found.
*/
std::string Response::getStatusMessage(int statusCode) const 
{
    std::map<int, std::string>::const_iterator it = _statusMsgs.find(statusCode);
    if (it != _statusMsgs.end()) 
        return it->second;
	else
        return "Unknown Status"; // Default message if not found
}

/* getErrorPage
   Retrieves the path to the error page associated with a specific HTTP status code.
   Parameters:
   - statusCode: The HTTP status code for which to retrieve the error page path.
   Returns: The path to the error page, defaults to a generic error page if specific not found.
*/
std::string Response::getErrorPage(int statusCode) const 
{
	std::string pagePath = this->_location.getErrPage(statusCode);
	if (!pagePath.empty()) 
		return (pagePath);
	else
		return "/error_pages/def_err.html"; // Default message if not found
}