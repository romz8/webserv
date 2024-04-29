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
Request::Request(const std::string& rawHead, const std::string& hostName, int maxBody) : _hostName(hostName), _maxBodySize(maxBody)
{
	std::string rawRequest;
	initRequest();
	if (rawHead.find("\r\n\r\n") == std::string::npos)
	{
		this->_status = 400;
		return ;
	}
	else
		rawRequest = rawHead.substr(0, rawHead.find("\r\n\r\n") + 4);
	try
	{
		parseHeader(rawRequest);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		this->_status = 400;
		std::cout << BG_RED << "ERROR IN Header" << RESET << std::endl;
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
		this->_method = src._method;
		this->_path = src._path;
		this->_version = src._version;
		this->_headers = src._headers;
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
	std::cout << BG_RED "REQUEST LINE is : " << line << RESET <<std::endl;
	std::istringstream lineStream(line);
	
	if (!isValidRL(line))
		throw std::runtime_error("Error parsing Request : invalid Request-Line on SP");
	lineStream >> this->_method >> this->_path >> this->_version;
}

/**
 * Validates the correctness of the request line based on HTTP standards.
 * @param line The request line to validate.
 * @return true if the request line is valid, false otherwise.
 */
bool Request::isValidRL(const std::string& line)
{
	const std::string SP  = " ";
	size_t firstspace, secondspace;
	
	if (line.empty())
		return (false);
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
	if ((nextsp != std::string::npos) && (nextsp != line[line.size() - 3]))
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
	if (it->second != this->_hostName)
		throw std::runtime_error("Error parsing Request : invalid Host header");
	return(true);
}

bool	Request::parseContentLenBody()
{
	std::cout << BG_GREEN "WE ARRIVE IN BODY PARSING" << std::endl;
	bool bodyok = false;
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
		if (len > this->_maxBodySize)
		{ 
			this->_status = 413;
			return(true);
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
	this->_hasCgi = false;
	this->_status = 0;
	this->_method.clear();
	this->_path.clear();
	this->_version.clear();
	this->_parsePath.clear();
	this->_body.clear();
	this->_respbody.clear();
	this->_extension.clear();
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
	sanitizeUrl();
	// std::cout << BG_CYAN << "status and path are " << this->_status << " and " << this->_path << std::endl;
	// std::cout << BG_CYAN << "method is " << this->_method << std::endl;
	// std::cout <<BG_CYAN << "Location is : " << _location.getPath() << RESET << std::endl;
	
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
	
	std::cout << RED "EXT IS : " << this->_extension << RESET << std::endl;
	std::cout << YELLOW "Request status-Line is : " << this->_method << " " << this->_path << " " << this->_version << RESET << std::endl;
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
	if (this->_method=="POST" && safeStrToSizeT(this->_headers["Content-Length"]) > _maxBodySize)
	{
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
 * Processes GET requests by validating the requested path and retrieving content if available.
 * This function first checks if the path is valid and accessible. If the request targets a directory,
 * it may return a directory listing or a specific index file if configured. The function sets
 * appropriate response codes based on the existence and accessibility of the target resource,
 * such as 404 for Not Found or 200 for successfully retrieving the content. It handles directory
 * path normalization by ensuring they end with a slash and redirecting if necessary.
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
	if (_location.hasCgi(parseExtension(this->_path, "")))
	{
		this->_path = _location.getPath() + this->_path.substr(_location.getPath().size());
		std::string execpath = _location.getCgiHandler(parseExtension(this->_path, ""));
		CGI cgi(*this, execpath);
		cgi.executeCGI();
		std::cout << BG_RED "CGI body is :" << cgi.getBody() << std::endl;
		this->_status = cgi.getStatus();
		if (this->_status == 200)
			this->_respbody = cgi.getBody();
		this->_hasCgi = true;
		std::cout << BG_GREEN << "CGI EXECUTED" << RESET << std::endl;
		std::cout <<"body is :" << _respbody << std::endl;
		return ;
	}
	if (this->_isDirectory)
	{
		std::string index = this->_location.getIndex();
		this->normalizeDirPath();
		if (this->_isDirNorm)
		{
			this->_status = 301;
			return;
		}
		else if (!index.empty() && fileExists(this->_location.getRootDir() + this->_path + index))
			this->_parsePath = _location.getRootDir() + this->_path + index;
		else if(_location.getAutoIndex() == true)
		{
			DirectoryListing dirList(this->_location.getRootDir() + this->_path);
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
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Content-Type");
	if (it == this->_headers.end() || it->second.empty())
	{
		this->_status = 400;
		return ;
	}
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

		std::string filePath = _location.getRootDir() + _location.getUploadFile() + ftime + ".txt";
		std::cout << BG_GREEN "Location is : " << _location.getPath() << std::endl;
		std::cout << "POST url FORM Ressource is : " << filePath << std::endl;
		std::cout << "Location uplaod is is : " << _location.getUploadFile() << RESET <<std::endl;
		std::ofstream file(filePath, std::ios::app);
		if (!file.is_open())
		{
			this->_status = 500;
			return ;
		}
		file << input << std::endl;
		file.close();
		this->_status = 201;
		this->_parsePath = loc.getRootDir() + loc.getPath();
		std::cout << BG_GREEN << "path is : " << this->_parsePath << RESET << std::endl;
}


void	Request::processMultipartForm(const std::string& input, const std::string& boundary)
{
	std::string delimiter = "--" + boundary;
	std::string	endDelimiter = delimiter + "--";
	std::string fname = "unknown.txt";
	size_t pos = 0, start = 0, end = 0;

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
	std::string filePath = _location.getRootDir() + _location.getUploadFile() + fname;
	std::cout << "POST UPLOAD file Ressource is : " << filePath << std::endl;
	std::ofstream file(filePath); //very testy ..update with Location and actual logic
	if (!file.is_open())
	{
		this->_status = 500;
		return;
	}
	file << body;
	file.close();
	this->_status = 201;
	this->_parsePath = _location.getRootDir() + _location.getUploadFile(); // TESTING
}

void	Request::processChunkBody(std::string input)
{
	std::string data;

	size_t pos = 0;
	data.clear();
	while (true)
	{
		size_t endBlock = input.find("\r\n", pos);
		size_t chunkSize = std::strtol(input.substr(pos, endBlock - pos).c_str(), NULL, 16);
		if (chunkSize == 0)
			break;
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
    std::cout << "DELETE METHOD PATH IS : " << this->_parsePath << std::endl;
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
	this->_parsePath = _location.getRootDir() + _location.getPath() + this->_path.substr(_location.getPath().size());
	std::cout << BG_YELLOW << "path is : " << this->_path << RESET << std::endl;
	std::cout << BG_YELLOW << "full path from loc is : " << this->_parsePath << RESET << std::endl;
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
	std::cout << BLUE "init url : " << url << std::endl;
	
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

	std::cout << BLUE "url post before /. check : " << url << std::endl;
	
	if (isHiddenAccess(url))
		url = "/";
	
	this->_path = url;
	std::cout << BLUE "url post ALL check : " << this->_path << std::endl;
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
		processChunkBody(body);
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
	std::cout << BG_GREEN "Location in REQUEST is : " << _location.getPath() << RESET << std::endl;

}

Location Request::getLocation() const
{
	return(this->_location);
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
	std::cout << "last of line is " << str[str.size() - 1] << " and " << str[str.size() - 2] << std::endl;
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


void	getQueryParams(const std::string& path, std::map<std::string, std::string>& params)
{
	size_t pos = path.find("?");
	if (pos == std::string::npos)
		return ;
	std::string query = path.substr(pos + 1);
	std::istringstream stream(query);
	std::string key, value;
	while (std::getline(stream, key, '=') && std::getline(stream, value, '&'))
		params[key] = value;
}	