/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:55:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/05 12:30:37 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/**
 * Constructor: Initializes a Request object by parsing the raw HTTP request string.
 * It sets up initial request parameters and handles exceptions during parsing,
 * assigning an error status as needed.
 *
 * @param rawRequest The raw HTTP request string.
 * @param hostName The host name from the HTTP request.
 * @param maxBody The maximum body size allowed for the request.
 */

Request::Request(const std::string host, \
const int maxBody, const std::vector<std::string> servName, const int port) : \
	_host(host), _maxBodySize(maxBody), _serverName(servName), _port(port)
{
	initRequest();
}

// Default constructor definition
Request::Request()
    : _host(""), _status(0), _maxBodySize(0),  _port(0), _HeaderRead(false), _HeaderOK(false) 
{
	_serverName.clear();
    initRequest();
}

bool	Request::processHeader(const std::string& rawHead)
{
	std::string rawRequest;
	if (rawHead.find("\r\n\r\n") == std::string::npos)
	{
		std::cerr << "Error parsing Request : no CRLF terminated at end of header" << std::endl;
		this->_status = 400;
		return (false);
	}
	else
		rawRequest = rawHead.substr(0, rawHead.find("\r\n\r\n") + 4);
	try
	{
		parseHeader(rawRequest);
		return (true);
	}
	catch(const std::exception& e)
	{
		std::cout << BG_RED << "ERROR IN Header" << RESET << std::endl;
		std::cerr << e.what() << std::endl;
		this->_status = 400;
		return (false);
	}
}

Request::~Request(){}

Request::Request(const Request& src)
{
	*this = src;
}

Request& Request::operator=(const Request& src)
{
	if (this != &src)
	{
		_host = src._host;
		_method = src._method;
		_path = src._path;
		_version = src._version;
		_isDirectory = src._isDirectory;
		_isDirNorm	= src._isDirNorm;
		_parsePath = src._parsePath;
		_extension = src._extension;
		_status = src._status;
		_maxBodySize = src._maxBodySize;
		_chunkBody = src._chunkBody;
		_rawinput = src._rawinput;
		_body = src._body;
		_respbody = src._respbody;
		_location = src._location;
	 	_execCgi = src._execCgi;
		_query = src._query;
		_headers = src._headers;
		_serverName = src._serverName;
		_port = src._port;
		_HeaderRead = src._HeaderRead;
		_HeaderOK = src._HeaderOK;
		_rawinput = src._rawinput;
		_rawBody = src._rawBody;
		_start = src._start;
		_cgi = src._cgi;
	}
	return (*this);
}

/*
*********************************************************************
**************** PARSING OF THE REQUEST HEADER ***********************
*********************************************************************
*/

/** 
 * Parses the HTTP request headers from a given string buffer (THE BIG BERTA of the Request parsing )
 *
 * @param head A string containing all the header lines read from a request.
 *
 * This function processes each line of the headers, validates that each is correctly formatted with
 * CRLF termination, and extracts key-value pairs from each header line. It checks the cumulative size
 * of the headers against a maximum allowable limit (8192 as per Nginx default setup on HTTP1.1)
 * to prevent potential denial-of-service attacks due to excessively large headers.
 *
 * Steps:
 * 1. Initializes a stream to sequentially read lines from the header string, check for lone CR.
 * 2. Skips any initial empty lines or lines consisting solely of whitespace.
 * 3. Reads and processes each header line:
 *    a. Checks if the line correctly ends with '\r' (the '\n' is consumed by getline).
 *    b. Removes the trailing '\r' for correct parsing.
 *    c. Ends reading headers upon reaching an empty line (headers are terminated by an empty line).
 *    d. Parses the header line to extract key-value pairs.
 *    e. Adds the length of the line to a cumulative counter to ensure it does not exceed the max limit.
 * 4. Throws exceptions if headers are not properly terminated or if individual lines are malformed.
 * 5. Validates that a "Host" header exists and is correct.
 *
 * @throws std::runtime_error If header lines are not properly formatted or exceed size limits.
 */
void Request::parseHeader(const std::string& head)
{
	std::istringstream stream(head);
	bool	headersFinished = false;
	std::string line;
	size_t	headerSize = 0;
	
	if (loneCR(head))
			throw std::runtime_error("Error parsing Request : lone CR found in header");
	while (std::getline(stream, line, '\n') && (line.empty() || line.find_first_not_of(" \t\n\r\f\v") == std::string::npos))
		continue;
	try
	{
		parseStartLine(line);
		line.clear();
		while (std::getline(stream, line,'\n'))
		{
			if (!line.empty() && line[line.size() - 1] != '\r')
				throw std::runtime_error("Error parsing Request : no CRLF terminated at line " + line);
			else
				line.erase(line.size() - 1);
			if (line.empty())
			{
				headersFinished = true;
				break;
			}
			parseHeaderLine(line);
			headerSize += line.size() + 2;
			if (headerSize > MAX_HEADER_SIZE)
				throw std::runtime_error("Error parsing Request : header size exceeded");
		}
		if (!headersFinished)
			throw std::runtime_error("Headers not properly terminated (Double CRLF)");
		hasCorrectHost();
	}
	catch(const std::exception& e)
	{
		throw;
	}
}

/**
 * Parses the start line of the HTTP request to extract the method, path, and version.
 * @param line The start line of the HTTP request.
 * @throws std::runtime_error if the start line is invalid.
 */
void Request::parseStartLine(const std::string& line)
{
	//std::cout << BG_RED "REQUEST LINE is : " << line << RESET <<std::endl;
	std::istringstream lineStream(line);
	
	if (!isValidRL(line))
		throw std::runtime_error("Error parsing Request : invalid Request-Line on SP");
	lineStream >> this->_method >> this->_path >> this->_version;
	std::cout << GREEN "Parsed Request-Line: Method: " << this->_method
          << ", Path: " << this->_path
          << ", Version: " RESET << this->_version << std::endl;
}

/**
 * Validates the correctness of the request line based on HTTP standards.
 * @param line The request line to validate.
 * @return true if the request line is valid, false otherwise.
 */
bool Request::isValidRL(const std::string& line)
{
	//std::cout << BLUE "line is : " RESET  << line << std::endl;
	const std::string SP  = " ";
	size_t firstspace, secondspace;
	
	if (line.empty() || line == "\r\n" || line == "\n" || line == "\r" || line == "\n\r" || line == "\r\n\r\n")
	{
		return (false);
		std::cout << BG_YELLOW "WTFFFF empty line" RESET << std::endl;
	}
	if (!endsWithCRLF(line))
        throw std::runtime_error("Request line does not end with CRLF");
	if (line[0] == SP[0] || line[line.size() - 1] == SP[0])
		return (false);
	firstspace = line.find(SP);
	if (firstspace == std::string::npos || firstspace == 0 || line[firstspace + 1] == ' ') //potential SF ?
		return (false);
	secondspace = line.find(SP, firstspace + 1); //should we not count number of space (only 2) and not consecutie ?
	if (secondspace == std::string::npos || secondspace == firstspace + 1) 
		return(false);
	size_t nextsp = line.find(SP, secondspace + 1);
	if ((nextsp != std::string::npos) && (nextsp != static_cast<size_t>(line[line.size() - 3])))
		return (false);
	for (size_t i = 0; i < line.size(); i++)
	{
		size_t cr = line.find("\r");
		if  (cr != std::string::npos)
		{
			if (cr != line.size() - 1 && line[cr + 1] != '\n')
				return (false);	
		}
	}
	return (true);
}

/**
 * Parses an individual HTTP header line to extract the key and value, ensuring correct formatting.
 *
 * @param line A single header line excluding the CRLF termination.
 * @throws std::runtime_error If the line is malformed (no colon, invalid whitespace).
 * This function validates the header line's format, checks for the presence of a colon (':') as
 * the delimiter between the key and value, and ensures there is no disallowed whitespace before
 * the colon. It then extracts the key and value, trims any surrounding whitespace, and stores them
 * in the header map.
 */
void	Request::parseHeaderLine(const std::string& line)
{
	if (line.empty())
		return;
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		throw std::runtime_error("Error parsing Request : no colon (:) at headerline " + line);
	std::string key = trim(line.substr(0, pos));
	if (key.empty())
		throw std::runtime_error("Error parsing Request : empty key in headerline " + line);
	std::string value = trim(line.substr(pos + 1));
	if (line.find_first_of(" \t") < pos)
		throw std::runtime_error("Whitespace found before colon in headerline" + line);
	this->_headers[key] = value;
}

/**
 * Checks if the 'Host' header is present and correct (as per RFC 9112)
 * @return true if the 'Host' header is correct, throws an exception otherwise.
 * @throws std::runtime_error if the 'Host' header is missing or incorrect.
 */
bool	Request::hasCorrectHost() const
{
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Host");
	if( it == this->_headers.end())
		throw std::runtime_error("Error parsing Request : no Host header");
	if (it->second.empty())
		throw std::runtime_error("Error parsing Request : empty Host header");
	if (it->second != this->_host && !foundStringinVec(it->second, this->_serverName))
	{
		std::cerr << "Host is : " << it->second << std::endl;
		std::cerr << "bad host is : " << this->_host << std::endl;
		// std::cerr << "servername is : " << this->_serverName << std::endl;
		// std::cerr << "test servername complete is : " << this->_serverName + ":" + std::to_string(this->_port) << std::endl;
		throw std::runtime_error("Error parsing Request : invalid Host header");
	}
	return(true);
}


bool	Request::parseContentLenBody()
{
	if (_headers["Content-Length"].empty())
	{
		this->_status = 400;
		return (false);
	}
	try
	{
		size_t len = safeStrToSizeT(_headers["Content-Length"]);
		if (len <= 0)
			this->_status = 400;
		std::cout << "Content-Length is : " << len << std::endl;
		std::cout << "maxBodySize is : " << this->_maxBodySize << std::endl;
		if (len > static_cast<size_t>(this->_maxBodySize))
		{ 
			this->_status = 413;
			return(false);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "overflow or error in content-length" << e.what() << std::endl;
		this->_status = 400;
		return (false);
	}
	return (true);
}

/**
 * Decides if the body should be parsed based on 'Content-Length' 
 * or 'Transfer-Encoding' headers.
 * For 'Content-Length', it checks if the specified length does not 
 * exceed the maximum allowed body size,
 * setting the status to 413 (Payload Too Large) if it does.
 *  If 'Transfer-Encoding' is set to 'chunked', it prepares the request 
 * for handling a chunked body. This function is key to handling HTTP persistence
 * and ensuring that the server can manage varying content sizes as specified by the client.
 */
bool Request::hasBody()
{
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Content-Length"); 
	if (it != this->_headers.end()  && it->second != "0")
		return (parseContentLenBody());
	it = this->_headers.find("Transfer-Encoding");
	if (it != this->_headers.end() && it->second == "chunked")
		return (true);
	return (false);
}

/**
 * Initializes a Request object to default states, 
 * clearing any existing data (if any).
 */
void	Request::initRequest()
{
	this->_isDirectory = false;
	this->_isDirNorm = false;
	this->_execCgi = false;
	this->_status = 0;
	this->_method.clear();
	this->_path.clear();
	this->_version.clear();
	this->_parsePath.clear();
	this->_body.clear();
	this->_respbody.clear();
	this->_extension.clear();
	this->_query.clear();
	this->_headers.clear();
	this->_HeaderOK = false;
	this->_HeaderRead = false;
	this->_rawinput.clear();
	this->_rawBody.clear();
	this->_start = -1;
	this->_cgi._env.clear();
	this->_cgi._path.clear();
	this->_cgi._body.clear();
	this->_cgi._respbody.clear();
	this->_cgi._execPath.clear();
	this->_cgi._onCGI = false;
	this->_cgi._start = -1;
}

/**
 *check if the headers alone (without body) contain a lone CR character.
 *to enforce RFC 9112 CRLF policy.
 * @param header The complete header string to check.
 * @return true if a lone CR is found, false otherwise.
 */
bool	loneCR(const std::string& header)
{
	std::string::const_iterator it = header.begin();
	for(;it != header.end(); it++)
	{
		if (*it == '\r')
		{
			if (it + 1 == header.end() || *(it + 1) != '\n')
				return (true);
		}
	}	
	return (false);

}

/*
*********************************************************************
********************* BUILD THE REQUEST **************************
*********************************************************************
*/
/*
 * Core of the engine after Request parsing
 * Completes the building of a Request by sanitizing the URL to prevent security vulnerabilities and
 * determining the appropriate HTTP response based on the method of the request.
 * This function orchestrates the response generation by checking if the request method is supported,
 * validating the request, and then delegating to the specific method handler (GET, POST, DELETE).
 * It handles errors by setting appropriate status codes when methods are unsupported.
*/
void	Request::buildRequest()
{
	
	//std::cout << BG_GREEN << "init path path is ; " << this->_path << RESET << std::endl;
	sanitizeUrl();
	getQueryParams();
	hexDecoding(_path); //CAREFULL SF (tested with /cgi-bin/hello.py?firstname=lljfamf&lastname=%3Bl%3Blf&address=%3B%3Blkfkdkf)

	StatusCode();
	if (this->_status >= 400)
		return; 
	if (this->_method == "POST") 
		this->handlePostRequest();
	else if (this->_method == "DELETE")
		this->handleDeleteRequest();
	else if (this->_method == "GET")
		this->handleGetRequest();
	else
		this->_status = 405;
	if (this->_status != 200)
		this->_extension = ".html";
	else
		this->_extension = parseExtension(this->_parsePath, ".html");
	
	//std::cout << RED "EXT IS : " << this->_extension << RESET << std::endl;
	//std::cout << YELLOW "Request status-Line is : " << this->_method << " " << this->_path << " " << this->_version << RESET << std::endl;
}

/**
 * Determines the appropriate HTTP status code for the Request based on its validity, method support,
 * and the server's ability to fulfill it. This function checks the HTTP method & version compatibility
 * It updates the status code to reflect issues such as unsupported methods (405),(505)
 *  then it compares request method and location endpoint settings to determine if the method and 
 * requirements are possible -> if not update object state to correct http status code
 */
void	Request::StatusCode() //later on add the location check and GET / POST / DELETE switch
{
	if (this->_status >= 400)
		return;
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
	if (this->_method == "GET" && _location.isMethodAllowed("GET")== false)
	{
		this->_status = 405;
		return ;
	}
	if (this->_method == "POST" && _location.isMethodAllowed("POST")== false)
	{
		this->_status = 405;
		return ;
	}
	if (this->_method == "DELETE" && _location.isMethodAllowed("DELETE")== false)
	{
		this->_status = 405;
		return ;
	}
	if (this->_method=="POST" && _location.getUploadAllowed() == false)
	{
		this->_status = 403;
		return ;
	}
	if (this->_method=="POST")
	{
		if (this->_headers.find("Content-Length") !=  this->_headers.end() && safeStrToSizeT(this->_headers["Content-Length"]) > static_cast<size_t>(_maxBodySize))
			this->_status = 413;
		return ;
	}
}

bool Request::isValidMethod() const
{
	if (this->_method == "GET" || this->_method == "POST" || this->_method == "DELETE")
		return (true);
	return (false);
}

bool Request::isValidVersion() const
{
	if (this->_version == "HTTP/1.1" || this->_version == "HTTP/1.0")
		return (true);
	return (false);
}


/*
*********************************************************************
*************************** GET Request Logic ***********************
*********************************************************************
*/

/**
 ON GENERAL LOGIC
 * Processes GET requests by validating the requested path and retrieving content if available.
 * This function first checks if the path is valid and accessible. If the request targets a directory,
 * it may return a directory listing or a specific index file if configured. The function sets
 * appropriate response codes based on the existence and accessibility of the target resource,
 * such as 404 for Not Found or 200 for successfully retrieving the content. It handles directory
 * path normalization by ensuring they end with a slash and redirecting if necessary.
 
 ON CGI Block
 * if the location object has a CGI handler configured for the requested extension, the function
 * adjusts the internal request path to match the filesystem path required for CGI script execution.
 *  
 * The function combines the base path configured for the location object with the remainder
 * of the requested path that comes after the location's base path ( in case the public URL 
 * does not directly correspond to the filesystem path on the server to access scripts).
 * 
 * Example:
 *  - If a CGI script is requested via the URL `http://example.com/cgi-bin/script.sh`, and the
 *    server is configured to handle requests under `/cgi-bin/` at a filesystem location like
 *    `/var/www/cgi-bin/`, this block will adjust the internal path to `/var/www/cgi-bin/script.sh`.
 * 
 * then it extract the executable path from the location object and creates a CGI object to execute the script.
 * then it sets the response status to the CGI object's status and retrieves the response body.
 * 
 */
void		Request::handleGetRequest()
{
	if (!isValidPath())
	{
		this->_status = 404;
		return ;
	}
	if (!hasReadAccess())
	{
		this->_status = 403;
		return ;
	}
	if (handleCgi())
		return ;
	if (this->_isDirectory)
	{
		std::string index = this->_location.getIndex();
		this->normalizeDirPath();
		if (this->_isDirNorm)
		{
			//std::cout << BG_YELLOW " rebuilt and REDIRECT TO : " RESET << this->_path << std::endl;
			this->_status = 301;
			return;
		}
		//std::cout << "looking for index file at : " << this->_parsePath + index << std::endl;
		if (!index.empty() && fileExists(this->_parsePath + index))
		{
            this->_parsePath.append(index);
            if (!hasReadAccess())
            {
                    this->_status = 403;
                    return;
            }
		}
		else if(_location.getAutoIndex() == true)
		{
			DirectoryListing dirList(this->_parsePath);
			this->_respbody = dirList.getHTMLListing();
			//return; ->otherwise no 200 status
		}
		else
		{
			this->_status = 404;
			return;
		}
	}
	this->_status = 200;
}

/*
*********************************************************************
*************************** POST Request Logic ***********************
*********************************************************************
*/

/**
 * Handles POST requests by processing submitted data according to the Content-Type.
 * This function supports different forms of data handling, including form submissions and file uploads.
 * It distinguishes between 'multipart/form-data' for file uploads and other types for simple form submissions.
 * Errors during processing set the status to 400 (Bad Request), while successful handling updates the status
 * to 201 (Created) or uses other appropriate success codes.
 */
void	Request::handlePostRequest()
{	
	std::map<std::string, std::string> data;
	std::string body = this->_body;
	//std::cout << "POST BODY IS : " << _body << std::endl;
	//std::cout << "RAW BODY IS : " << _rawBody << std::endl;
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Content-Type");
	if (it == this->_headers.end() || it->second.empty())
	{
		this->_status = 400;
		return ;
	}
	if (handleCgi())
		return ;	
	if (it->second.find("multipart/form-data") != std::string::npos)
	{
		std::string boundary = extractBoundary(this->_headers["Content-Type"]);
		if (boundary.empty())
		{
			this->_status = 400;
			return ;
		}
		processMultipartForm(body, boundary);
	}
	else
		processFormData(body, this->_location);
}

void	Request::processFormData(const std::string& input, const Location& loc)
{
		std::istringstream stream(input);
		std::string ftime = formattedTime();

		std::string filePath = _location.getUploadFile() + ftime + ".txt";
		//std::cout << BG_GREEN "Location is : " << _location.getPath() << std::endl;
		std::cout << BLUE "POST url FORM Ressource is : " RESET<< filePath << std::endl;
		//std::cout << "Location uplaod is is : " << _location.getUploadFile() << RESET <<std::endl;
		std::ofstream file(filePath.c_str(), std::ios::app);
		if (!file.is_open())
		{
			this->_status = 500;
			return ;
		}
		file << input << std::endl;
		file.close();
		this->_status = 201;
		this->_parsePath = loc.getRootDir() + loc.getPath();
}


void	Request::processMultipartForm(const std::string& input, const std::string& boundary)
{
	std::cout << " IN MULTIPART FORM with boundary :"<< boundary << std::endl;
	std::string delimiter = "--" + boundary;
	std::string	endDelimiter = delimiter + "--";
	std::string fname = "unknown.txt";
	size_t start = 0, end = 0;

	start += delimiter.length() + 2; 
	end = input.find(endDelimiter) - 2;
	std::string part = input.substr(start, end - start);
	size_t headerpos= part.find("\r\n\r\n");	
	std::string headers = part.substr(0, headerpos);
	std::string body = part.substr(headerpos + 4);
	
	std::istringstream headStream(headers);
	std::string line;
	while(std::getline(headStream, line) && !line.empty())
	{
		if (line.find("filename=") != std::string::npos)
		{
			size_t pos = line.find("filename=\"") + 10;
			size_t bracketpos = line.substr(pos).find('\"');
			fname = line.substr(pos, bracketpos);
		}
	}
	std::string filePath = _location.getUploadFile() + fname;
	//std::cout << "POST UPLOAD file Ressource is : " << filePath << std::endl;
	std::ofstream file(filePath.c_str()); //very testy ..update with Location and actual logic
	if (!file.is_open())
	{
		std::cerr << "Error opening file" << std::endl;
		std::cerr << "File path is : " << filePath << std::endl;
		this->_status = 500;
		return;
	}
	file << body;
	file.close();
	this->_status = 201;
	this->_parsePath = _location.getRootDir() + _location.getUploadFile(); // TESTING
}

void	Request::parseChunkBody(const std::string& input)
{
	std::string data;
	size_t totalSize = 0;
	size_t pos = 0;
	
	data.clear();
	while (true)
	{
		size_t endBlock = input.find("\r\n", pos);
		long chunkSize = std::strtol(input.substr(pos, endBlock - pos).c_str(), NULL, 16);
		std::cout << "CHUNK SIZE IS : " << chunkSize << std::endl;
		if (chunkSize == 0)
			break;
		totalSize += chunkSize;
		if (totalSize > static_cast<size_t>(_maxBodySize))
		{
			this->_status = 413;
			return;
		}
		std::string unchunked = input.substr(endBlock + 2, chunkSize);
		data.append(unchunked);
		pos = endBlock + chunkSize + 2 + 2;
	}
	this->_body = data;
}

/*
*********************************************************************
*************************** DELETE Request Logic ********************
*********************************************************************
*/

/**
 Handles a DELETE HTTP request to remove a specified resource from the server.
 This function is designed to delete both files and empty directories based on
 the request path provided. It performs several key operations:

 1. Path Resolution: Converts the sanitized URL path into a valid server path using predefined 
    Location mappings. This helps in locating the exact file or directory on the server.
 
 2. Resource Existence and Type Verification: Utilizes the stat() system call to ascertain
    whether the target path is a directory or a file. It sets a 404 Not Found status if the path 
    does not exist or is invalid.
 
 3. Deletion Logic:
    - For directories: Checks if the directory is empty (containing only '.' and '..'). If empty,
      it attempts to delete the directory. If not empty, it sets a 409 Conflict status indicating
      that the directory cannot be deleted unless it is empty.
    - For files: Attempts to delete the file directly and handles failure by setting appropriate 
      HTTP status codes.

 The function modifies the HTTP response status based on the outcome of the delete operation,
 providing clear and RESTful feedback to the client.
 */
void	Request::handleDeleteRequest()
{
	
	this->_parsePath = _location.getPath() + this->_path.substr(_location.getPath().size());
    if (!isValidPath())
	{
        _status = 404;  
        return;
    }
	if (!hasWriteAccess())
	{
		this->_status = 403;
		return;
	}

	if(this->_isDirectory)
		DeleteDirectory();
	else
	{
		if(!deleteResource(this->_parsePath))
			this->_status = 500;
		else
			this->_status = 204;
	}
}

void Request::DeleteDirectory()
{
	std::string path = this->_parsePath;
	bool emptyDir = true;

	if (path[path.size() - 1] != '/')
	{
		this->_status = 409;
		return;
	}
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
	{
		this->_status = 404;
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name != "." && name != "..")
			emptyDir = false;
	}
	closedir(dir);
	if (!emptyDir)
	{
		this->_status = 409;
		return;
	}
	if (!deleteResource(path))
		this->_status = 500;
	else
		this->_status = 204;
}

/*
*********************************************************************
*************************** CGI Logic ***************************
*********************************************************************
*/

bool	Request::handleCgi()
{
	if (_location.hasCgi(parseExtension(this->_path, "")))
	{
		this->_parsePath = _location.getRootDir() + _location.getPath() + this->_path.substr(_location.getPath().size());
		std::string execpath = _location.getCgiHandler(parseExtension(this->_path, ""));
		if (_method == "POST")
			_query = _body;		
		initCgi(execpath);
		executeCGI();
		this->_execCgi = true;
		return (true);
	}
	return (false);
}
/*
*********************************************************************
*************************** PATH VALIDATION ***************************
*********************************************************************
*/
/*
verify that path is ok at first (not empty, starting with /) then,
based on the obejct state (path) -> update the _parsePath with the full path
based on the location endpoint path and root settings.
then check if the path exists and if is either a file or a dir (with stat())
if issue -> returning false
otherwise return true
*/
bool Request::isValidPath() 
{
	if (this->_path.empty() || this->_path[0] != '/')
		return (false);
	this->_parsePath = _location.getRootDir() + this->_path.substr(_location.getPath().size());
	std::cout << BLUE << "path is : " << this->_path << RESET << std::endl;
	std::cout << BLUE << "full path from loc is : " << this->_parsePath << RESET << std::endl;
	struct stat path_stat;
	if (stat(this->_parsePath.c_str(), &path_stat) == -1)
		return(false);
	if(S_ISDIR(path_stat.st_mode))
		this->_isDirectory = true;
	else if(S_ISREG(path_stat.st_mode))
		this->_isDirectory = false;
	else
		return(false);
	return (true);
}

/*
if a path leads to a valid (existing) dir, we check it does end by '/', 
if not we add the / and redirect it to the correct path/dir/ address by
returning a 301 with path to the browser
this is necessary as stat doesn't check this but http requires this management
*/
void Request::normalizeDirPath()
{
	std::string path = this->_parsePath;
	if (path[path.size() - 1] == '/')
		return ;
	this->_isDirNorm = true;
	this->_parsePath = this->_path.append("/");
	return ;
}


// R_OK tests for readability only
bool Request::hasReadAccess() const
{	
    if (access(this->_parsePath.c_str(), R_OK) == 0)
		return(true);
	return (false); 
}

// W_OK tests for write only
bool Request::hasWriteAccess() const 
{
    return (access(this->_parsePath.c_str(), W_OK) == 0);
}

// W_OK tests for write only
bool Request::hasExecAccess() const 
{
    return (access(this->_parsePath.c_str(), X_OK) == 0);
}

/*
********************************************************************
*********************** URI SANITIZATION ***********************
*********************************************************************
*/

/*
Remove occurrences of '/../' to prevent directory traversal -> BRowser do it automiatcally, test with POSTMAN
Change '//' to '/' to avoid a url to go up the env to filesys
Block access to hidden files and directories (any /. (like /.env) -> redirect to root)
*/
void	Request::sanitizeUrl()
{
	std::string url = this->_path;
	//std::cout << BLUE "init url : " << url << std::endl;
	
	size_t pos;
	pos = 0;
	while ((pos = url.find("/../") != std::string::npos)) // ACTUALLY HANDLED BY BROWSER BUT VALID : SHOULD COMPARE IF EXCEED ROOT
		url.erase(pos, 3);
	
	pos = 0;
	while ((pos = url.find("//", pos)) != std::string::npos)
	{
		url.replace(pos, 2, "/");
		pos += 1;
	}

	//std::cout << BLUE "url post before /. check : " << url << std::endl;
	
	if (isHiddenAccess(url))
		url = "/";
	
	this->_path = url;
	//std::cout << BLUE "url post ALL check : " << this->_path << std::endl;
}

/*
 * Checks if the given URL attempts to access a hidden file or directory.
 * 
 * Hidden files or directories in Unix-based systems start with a dot (.) like .env
 * This function searches for occurrences of "/." in the URL,
 * However, it's important to distinguish between legitimate navigational patterns
 * like "../" (navigate up one directory) or "./" (current directory) and actual
 * attempts to access hidden resources. Therefore, this function specifically looks
 * for "/." patterns that do not form part of these legitimate patterns.
 * 
 * The function iterates through the URL, searching for "/." sequences. When such
 * a sequence is found, the function checks the character immediately following "/.".
 * If the character is neither a slash (/) nor a dot (.), it indicates an attempt to
 * access a hidden file or directory. In such cases, the function returns true, signaling
 * that the URL should be sanitized or blocked to prevent unauthorized access.
 * 
 * If the sequence "/." is found at the end of the URL or is followed by a slash or another
 * dot, it's considered part of a legitimate navigational pattern, and the search continues.
 * The function returns false if no unauthorized attempt to access a hidden resource is detected.
 */
bool Request::isHiddenAccess(const std::string& url) 
{
    size_t pos = 0;
    while ((pos = url.find("/.", pos)) != std::string::npos) {
        if (pos + 2 == url.length() || (url[pos + 2] != '/' && url[pos + 2] != '.'))  // "/." at the end of the URL and Not followed by another dot or slash
            return true; 
        pos += 2; // Move past the "/." sequence to continue checking
    }
    return false; // No hidden file/directory access detected
}

/*
*********************************************************************
*********************** GETTERS & SETTERS ***********************
*********************************************************************
*/

std::string Request::getMethod() const
{
	return (this->_method);
}

std::string Request::getPath() const
{
	return (this->_path);
}

std::string Request::getParsePath() const
{
	return (this->_parsePath);
}

std::string Request::getExtension() const
{
	return (this->_extension);
}

int Request::getStatus() const
{
	return (this->_status);
}

std::map<std::string, std::string> Request::getHeader() const
{
	return (this->_headers);
}

std::string Request::getHeaderField(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find(key);
	if (it != this->_headers.end())
		return (it->second);
	return ("");
}

/**
 * based on header parsing, headers map building and read and if no 413
 * it will then process the body from a string accordingly to the header
 */
void Request::setBody(const std::string& body)
{
	if (this->_status == 413)
		return;
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Transfer-Encoding");
	if (it != this->_headers.end() && it->second == "chunked")
		parseChunkBody(body);
	else
		this->_body = body;
}

std::string  Request::getBody() const
{
	return(this->_body);
}

std::string  Request::getrespBody() const
{
	return(this->_respbody);
}

void	Request::setStatus(int status)
{
	this->_status = status;
}

void	Request::setLocation(const Location& loc)
{
	this->_location = loc;
}

Location Request::getLocation() const
{
	return(this->_location);
}

std::string Request::getQueryString() const
{
	return(this->_query);
}

bool Request::execCgi() const
{
	return(this->_execCgi);
}

void	Request::setHost(const std::string& host)
{
	this->_host = host;
}

std::string Request::getHost() const
{
	return(this->_host);
}

void	Request::setServerName(const std::vector<std::string>& servername)
{
	this->_serverName = servername;
}

std::vector<std::string> Request::getServerName() const
{
	return(this->_serverName);
}

void	Request::setPort(int serverport)
{
	this->_port = serverport;
}

int Request::getPort() const
{
	return(this->_port);
}

/*
only used in case of alias / redirection with 301 bypassing the buildRequest method
*/
void	Request::setPath(const std::string& path)
{
	this->_parsePath = path;
}

std::string Request::getrawBody() const
{
	return(this->_rawBody);
}

t_cgi&	Request::getCgi()
{
	return(this->_cgi);
}
/************************** UTILS **********************************/

/*
This function looks for the "boundary=" substring within the Content-Type header and
 returns the boundary value. If the boundary is quoted, it 
 properly extracts the quoted string.
 */
std::string extractBoundary(const std::string& contentType) 
{
    std::string boundaryPrefix = "boundary=";
    size_t pos = contentType.find(boundaryPrefix);
    if (pos != std::string::npos) {
        pos += boundaryPrefix.length();
        if (contentType[pos] == '\"') {
            pos++;
            return contentType.substr(pos, contentType.find('\"', pos) - pos);
        }
        return contentType.substr(pos);
    }
    return "";
}

/*
utils to trim headers after Browser parsing
*/
std::string trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

//extract exttension (.something) from a path
std::string parseExtension(const std::string& path, const std::string& def)
{
	if (path.empty())
		return(def);
	size_t pos = path.find_last_of('.');
	if (pos != std::string::npos)
    	return(path.substr(pos));
	else
		return(def);
}

void Request::printRequest() const
{
	std::cout << BG_YELLOW "Method : " << this->_method << std::endl;
	std::cout << "status is : " << this->_status << std::endl;
	std::cout << "Path : " << this->_path << std::endl;
	std::cout << "Version : " << this->_version << std::endl;
	std::cout << "Extension is  : " << this->_extension << std::endl;
	std::cout << "is Dir  : " << this->_isDirectory << std::endl;
	std::cout << "is DirNorm  : " << this->_isDirNorm << std::endl;
	std::cout << "Requests : " << std::endl;

	std::cout << RESET << std::endl;
	//std::cout << BG_CYAN "Body is : " << this->_body << RESET << std::endl;
}


void Request::printHeader() const
{
	std::map<std::string, std::string>::const_iterator it;
	for(it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		std::cout << BG_CYAN << it->first << " : " << it->second << RESET << std::endl;
	}
}

//check if the last character of a string is a CRLF
bool	endsWithCRLF(const std::string& str) 
{
    if (str.size() < 3) 
		return false;
    return (str[str.size() - 1] == '\r');
}

//check if there is a consecutive space in a string
bool hasConsecutiveSpace(const std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
    {
        if (std::isspace(str[i]) && std::isspace(str[i + 1]))
            return true;
    }
    return false;
} 

//remove ressource from the filesystem, raise an error if not possible
bool deleteResource(const std::string& path) 
{
    if (std::remove(path.c_str()) == 0)
        return true;
    else
        return false;
}

//check if a file exists in the filesystem
bool fileExists(const std::string& path) 
{
    struct stat buffer;   
    return (stat(path.c_str(), &buffer) == 0);  // Use stat to check for file existence
}

//safe conversion from string to size_t -> avoid overflow, non-numeric input and underflow
size_t safeStrToSizeT(const std::string& str) 
{
    char* end;
    errno = 0; 
    long val = std::strtol(str.c_str(), &end, 10);

    if (end == str.c_str() || *end != '\0' || errno == ERANGE || val < 0) 
	{
        if (val < 0 || errno == ERANGE)
            throw std::runtime_error("Overflow or underflow occurred in strtol conversion");
        throw std::runtime_error("Conversion error occurred in strtol");
    }
    return (static_cast<size_t>(val));
}

//get the current time in a formatted string (YYYY-MM-DD_HH-MM-SS)
std::string formattedTime() {
    char buffer[80];
    time_t now = time(0);
    struct tm *timeinfo = localtime(&now);
    strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", timeinfo);
    return (std::string(buffer));
}

//is this good to split with params or should we only split the between query and path
//and therefore only save the _query = path
//careful to put it before "isvalidpath and on _path to clean ressoruce lookup in get"
/*
* with query == "/cgi-bin/script.sh?name=hi"
* will be query == "name=hi" and name == "/cgi-bin/script.sh"
*/
void	Request::getQueryParams()
{
	//std::cout << "before entering the Query Parmas we have : " << _path << std::endl;
	size_t pos = _path.find("?");
	if (pos == std::string::npos)
		return ;
	std::string query = _path.substr(pos + 1);
	std::istringstream stream(query);
	this->_query = query;
	this->_path = _path.substr(0, pos);
	//std::cout << BG_GREEN "After Query Parmas we have : " << _path << std::endl;
	//std::cout << "and query : " << _query << RESET << std::endl;
}

/**
 * Decodes percent-encoded characters within a URL.
 *
 * scans through the provided URL string and replaces any
 * percent-encoded sequences (e.g., "%20" for space) with their corresponding
 * ASCII characters. It also converts any occurrences of the '+' character
 * to spaces, as '+' is used to represent spaces in URL query parameters.
 *
 * @param url A reference to the string containing the URL to be decoded.
 *
 * Example usage:
 * std::string url = "http://example.com/search?q=C%2B%2B";
 * hexDecoding(url);
 * // Output: http://example.com/search?q=C++
 */
void hexDecoding(std::string& url)
{
	//std::cout << "Pre hexDecoding url is : " << url << std::endl;
	size_t hexpos = 0, pos = 0;

	while (( hexpos = url.find("%", hexpos)) != std::string::npos)
	{
		if (hexpos + 3 > url.size())
			break;
		std::string tmp = url.substr(hexpos + 1, 2); //why npos in std::string tmp = subName.substr(pos + 1, std::string::npos);? there are only 3 no ?
		long c = strtol((char *) tmp.c_str(), NULL, 16);
		const char c_char = static_cast<char> (c);
		std::cout << BG_GREEN "we are replacing : " << tmp << " with " << c_char << RESET << " for long value : " << c << std::endl;
		url.erase(hexpos, 3);
		url.insert(hexpos, 1, c_char);
	}

	while ((pos = url.find("+", pos)) != std::string::npos)
		url.replace(pos, 1, " ");

	//std::cout << "After hexDecoding url is : " << url << std::endl;
}


bool	Request::_readRequest(char* buffer, int byteSize)
{

	byteUpload(buffer, byteSize);
	std::string input = _rawinput;
	if (!_HeaderRead)
	{
		_HeaderOK = processHeader(input);
		if (input.find("\r\n\r\n") != std::string::npos)
		{
			_HeaderRead = true;
			//_HeaderOK = processHeader(input);
			std::string tempBody = input.substr(input.find("\r\n\r\n") + 4);
			// _rawinput.clear();
			// _rawinput.append(tempBody);
			_rawBody.append(tempBody);
		}
		else
			return (true);
	}
	if (_HeaderRead && !_HeaderOK)
	{
		std::cout << "Header not ok" << std::endl;
		return (true);
	}
	else if (_HeaderRead && _HeaderOK )
	{
		return(parseBody());
	}
	std::cout << "Header not read" << std::endl;
	return (false);
}

bool Request::processChunkBody(std::string buffer) 
{
	std::cout << "arrived in chunk body" << std::endl;
	bool endChunk = (buffer.find("0\r\n\r\n") != std::string::npos);
	_chunkBody.append(buffer);
	return (endChunk);
}

/**
 * @brief Reads the HTTP body from a client socket.
 * 
 * Reads the body of the HTTP request based on the content length or transfer encoding.
 * 
 * @param io_socket The client socket file descriptor.
 * @param header A map containing the HTTP headers previously parsed.
 * @param rawhead The raw HTTP header - used to rebuild the body if header read had partial body.
 * @param body A reference to the string where the body will be stored.
 * @return 1 on successful read, 0 on client disconnection, -1 on error, -2 on timeout.
 */
bool Request::parseBody(void)
{

	try
	{
		if (_headers.find("Transfer-Encoding") != _headers.end() && _headers.find("Transfer-Encoding")->second == "chunked")
			return (processChunkBody(_rawBody));
		else if (_headers.find("Content-Length") != _headers.end())
		{
			if (_rawBody.size() == safeStrToSizeT(_headers["Content-Length"]))
			{
				std::cout << BG_GREEN "all parsed" << RESET << std::endl;
				return (true);
			}
			else
			{
				if (_rawBody.size() > static_cast<size_t>(_maxBodySize))
					std::cout << "body too big Max is : " << _maxBodySize << " and current is : " << _rawBody.size() << std::endl;
				std::cout << BG_RED "not all parsed" << RESET << std::endl;
				std::cout << "body size is : " << _rawBody.size() << " and content length is : " << safeStrToSizeT(_headers["Content-Length"]) << std::endl;
				std::cout << " read left is : " << safeStrToSizeT(_headers["Content-Length"]) - _rawBody.size() << std::endl;
				return (false);
			}
		}
		else
			return (true);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		this->_status = 400;
		return (true);
	}
	
	
}


std::ostream& operator<<(std::ostream& os, const Request& req)
{
	os << YELLOW "Method : " << req.getMethod() << std::endl;
	os << "Path : " << req.getPath() << std::endl;
	os << "Version : " << req._version << std::endl;
	os << "Host : " << req.getHost() << std::endl;
	os << "PORT : " << req.getPort() << std::endl;
	// os << "ServerName : " << req.getServerName() << std::endl;
	os << "max body size : " << req._maxBodySize << std::endl;
	os << "Extension is  : " << req.getExtension() << std::endl;
	os << "is Dir  : " << req._isDirectory << std::endl;
	os << "is DirNorm  : " << req._isDirNorm << std::endl;
	os << "Request Header : " << std::endl;
	std::map<std::string, std::string>::const_iterator it;
	for(it = req._headers.begin(); it != req._headers.end(); ++it)
	{
		os << CYAN << it->first << " : " << it->second << RESET << std::endl;
	}
	os << RESET << std::endl;
	return (os);
}

void	Request::byteUpload(char *buffer, int byteSize)
{
	for (int i = 0; i < byteSize; i++)
	{
		if (!_HeaderRead)
			_rawinput.push_back(buffer[i]);
		else
			_rawBody.push_back(buffer[i]);
	}
}

bool Request::foundStringinVec(const std::string& target, const std::vector<std::string>& vec) const
{
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
	{
		if ((*it + ":" + std::to_string(this->_port)) == target)
			return(true);
	}
	return(false);
}