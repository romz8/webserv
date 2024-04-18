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

/* Constructor: Initializes the Request object by parsing the provided raw HTTP 
Request string. It sets up initial request parameters and catches any exceptions 
during parsing, setting an error status if needed.
*/
Request::Request(const std::string& rawRequest, const std::string& hostName, int maxBody) : _hostName(hostName), _maxBodySize(maxBody)
{
	initRequest();
	try
	{
		parseHeader(rawRequest);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		this->_status = 400;
		std::cout << BG_GREEN << "ERROR IS Header" << RESET << std::endl;
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
/*
first we parse the start line (containing the method, path and version)
after skipping any empty lines or lines that contain only whitespace or CRLF
then we parse the Requests as key value pairs inside a map
we implement a trimming for key, value for insrting in the map
*/
void Request::parseHeader(const std::string& head)
{
	std::istringstream stream(head);
	bool	headersFinished = false;
	std::string line;
	
	while (std::getline(stream, line, '\n') && (line.empty() || line.find_first_not_of(" \t\n\r\f\v") == std::string::npos))
		continue;
	try
	{
		parseStartLine(line);
		while (std::getline(stream, line,'\n'))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (line.empty())
			{
				headersFinished = true;
				break;
			}
			parseHeaderLine(line);
		}
		if (!headersFinished)
			throw std::runtime_error("Headers not properly terminated (CRLFCRLF)");
		hasCorrectHost();
	}
	catch(const std::exception& e)
	{
		throw;
	}
}

// Extracts the HTTP method, path, and version from the start line of the request.
void Request::parseStartLine(const std::string& line)
{
	std::cout << BG_RED "line is : " << line << RESET <<std::endl;
	std::istringstream lineStream(line);
	
	if (!isValidRL(line))
		throw std::runtime_error("Error parsing Request : invalid Request-Line on SP");
	lineStream >> this->_method >> this->_path >> this->_version;
}
/*
Parses a single Request line, extracting the Request name and value, and storing them after trimming whitespace.
Skip empty lines to prevent parsing errors, then Locate the delimiter between Request name and value.
If no delimiter is found, the Request line is malformed. We then extract and trim the Request name and value, 
storing them in the Request map. Finally Store the key-value pair in the Request map
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
	if (firstspace == std::string::npos || firstspace == 0 || line[firstspace + 1] == ' ')
		return (false);
	secondspace = line.find(SP, firstspace + 1);
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


void	Request::parseHeaderLine(const std::string& line)
{
	if (line.empty())
		return;
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		throw std::runtime_error("Error parsing Request : no colon (:) at line " + line);
	std::string key = trim(line.substr(0, pos));
	std::string value = trim(line.substr(pos + 1));
	this->_headers[key] = value;
}

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

void	Request::initRequest()
{
	this->_isDirectory = false;
	this->_isDirNorm = false;
	this->_status = 0;
	this->_method.clear();
	this->_path.clear();
	this->_version.clear();
	this->_parsePath.clear();
	this->_body.clear();
	this->_extension.clear();
}

/*
*********************************************************************
********************* BUILD THE REQUEST **************************
*********************************************************************
*/
/*
Core of the engine after Request parsing
*/
void	Request::buildRequest()
{
	std::cout << BG_BLUE "INIT _PATH : " << this->_path << RESET << std::endl;
	sanitizeUrl();
	if (this->_status != 400) //later on add the location check and GET / POST / DELETE switch
		setStatus();
	if (this->_method == "POST") //later on add the location check
		this->handlePostRequest();
	if (this->_status != 200)
		this->_extension = ".html";
	else
		this->_extension = parseExtension(this->_parsePath, ".html");
	std::cout << RED "EXT IS : " << this->_extension << RESET << std::endl;
	std::cout << YELLOW "Request status-Line is : " << this->_method << " " << this->_path << " " << this->_version << RESET << std::endl;
}

/*
Analyzes the HTTP request details stored in the Request object to determine the appropriate
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
void	Request::setStatus()
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
*************************** POST Request Logic ***********************
*********************************************************************
*/

void	Request::handlePostRequest()
{
	std::vector<Location> loc;
	loc.push_back(Location("/", "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/"));
	//loc.push_back(Location("/", "/Users/romainjobert/Desktop/42/Webserv/proto/html/"));
	
	std::map<std::string, std::string> data;
	std::string body = this->_body;
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Content-Type");
	if (it == this->_headers.end())
	{
		this->_status = 400;
		return ;
	}
	if (it->second == "application/x-www-form-urlencoded")
	{
		std::cout << BG_GREEN "WE ARE PROCESSING A FORM DATA" RESET << std::endl;
		processFormData(body, loc[0]);
	}
	
	if (it->second.find("multipart/form-data") != std::string::npos)
	{
		std::cout << BG_GREEN "WE ARE PROCESSING A MUTLIPART DATA" RESET << std::endl;
		std::string boundary = extractBoundary(this->_headers["Content-Type"]);
		processMultiFormat(body, boundary);
	}
	std::cout << BG_GREEN << "STATUS AT POST EXIT IS : " << this->_status << RESET << std::endl;
}
void	Request::processFormData(const std::string& input, const Location& loc)
{
		std::string key;
		std::string value;
		std::istringstream stream(input);
		std::string body;
		std::map<std::string, std::string> data;

		while (std::getline(stream, body, '&'))
		{
			size_t pos = body.find("=");
			if (pos != std::string::npos)
			{
				key = body.substr(0, pos);
				value = body.substr(pos + 1);
				data[key] = value;
			}
		}
		std::ofstream file(loc.getPath() + this->_parsePath + "post_data.txt", std::ios::app); //very testy ..update with Location and actual logic
		if (!file.is_open())
		{
			this->_status = 500;
			return ;
		}
		std::cout << BG_GREEN << "OPENED FILE : " << RESET << std::endl;
		std::map<std::string, std::string>::const_iterator it;
		for(it = data.begin(); it != data.end(); ++it)
		{
			file << it->first << " : " << it->second << std::endl;
		}
		file.close();
		this->_status = 201;
}



void	Request::parseContentLenBody(const std::string& request)
{
	int	len = atoi(_headers["Content-Length"].c_str());
	if (len < 0)
	{
		this->_status = 400;
		std::cout << BG_GREEN << "CONTENT LEN 0" << RESET << std::endl;
		return;
	}
	std::cout << BG_RED << "body is :" << this->_body << RESET << std::endl;
	if (_body.empty() && len > 0)
	{
		this->_status = 400;
		std::cout << BG_GREEN << "ERROR IS empty body AND LEN > 0" << RESET << std::endl;
	}
	if (_body.empty())
	{
		this->_status = 400;
		std::cout << BG_GREEN << "ERROR IS empty body" << RESET << std::endl;
	}
	else if (_body.size() > this->_maxBodySize)
		this->_status = 413;
	else if (_body.size() < atoi(_headers["Content-Length"].c_str()))
	{
		this->_status = 400;
		std::cout << BG_GREEN << "ERROR IS  body < LEN"; 
		std::cout << "body is :" << _body.size() << "and CL is " << _headers["Content-Length"] << RESET << std::endl;
	}
}


void	Request::processMultiFormat(const std::string& input, const std::string& boundary)
{
	std::string delimiter = "--" + boundary;
	std::string	endDelimiter = delimiter + "--";
	std::string fname;
	size_t pos = 0, start = 0, end = 0;

	start += delimiter.length() + 2; //for CRLF
	end = input.find(endDelimiter, start) - 2; //for CRLF (to not select)

	std::string part = input.substr(start, end - start);
	size_t headerpos= part.find("\r\n\r\n");
	
	std::string headers = part.substr(0, headerpos);
	std::string body = part.substr(headerpos + 4);
	std::cout << BG_RED << "headers are : " << headers << RESET << std::endl;
	
	std::istringstream headStream(headers);
	std::string line;
	while(std::getline(headStream, line) && !line.empty())
	{
		if (line.find("Content-Disposition:"))
		{
			size_t npos = line.find("filename=");
			if (npos == std::string::npos)
				fname = "unknown.txt";
			else
				fname = line.substr(npos + 9);
			std::cout << BG_RED << "fname is : " << fname << RESET << std::endl;
		}
	}
	std::vector<Location> loc;
	loc.push_back(Location("/", "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/"));
	std::cout << BG_GREEN << "FILE PATH IS : " << loc[0].getPath() + "upload/" + fname << RESET << std::endl;
	std::ofstream file(loc[0].getPath() + "upload/" + fname); //very testy ..update with Location and actual logic
	file << body;
	file.close();
	this->_status = 201;
}
/*
*********************************************************************
*************************** PATH VALIDATION ***************************
*********************************************************************
*/
/*
verify that path is ok at first (not empty, starting with /)
then check if the path is either a file or a dir (with stat())
if neither -> raiseError by returning null, otherwise update Response State
*/
bool Request::isValidPath() 
{
	std::vector<Location> loc;
	//loc.push_back(Location("/", "/Users/romainjobert/Desktop/42/Webserv/proto/html/"));
	//loc.push_back(Location("/recipe/", "/Users/romainjobert/Desktop/42/Webserv/proto/html/asset/"));
	loc.push_back(Location("/", "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/"));
	loc.push_back(Location("/recipe/", "/Users/rjobert/Desktop/42_cursus/webserv/proto/html/asset/"));
	
	if (this->_path.empty() || this->_path[0] != '/')
		return (false);
	if (loc[0].match(this->_path))
	{
		this->_parsePath = loc[0].getPath() + this->_path.substr(loc[0].getPrefixSize());
		std::cout << BG_YELLOW << "full path from loc is : " << this->_parsePath << RESET << std::endl;
		std::cout << BG_YELLOW << "full path from loc is from root 1" << std::endl;
	}
	if (loc[1].match(this->_path))
	{
		this->_parsePath = loc[1].getPath() + this->_path.substr(loc[1].getPrefixSize());
		std::cout << BG_YELLOW << "full path from loc is : " << this->_parsePath << RESET << std::endl;
		std::cout << BG_YELLOW << "full path from loc is from root 2" << std::endl;
	}
	std::cout << BG_YELLOW << "path is : " << this->_path << RESET << std::endl;
	
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

bool Request::hasBody() const
{
	std::map<std::string, std::string>::const_iterator it;
	it = this->_headers.find("Content-Length");
	if (it != this->_headers.end())
		if (it->second != "" && atoi(it->second.c_str()) > 0)
			return (true);
	it = this->_headers.find("Transfer-Encoding");
	if (it != this->_headers.end() && it->second == "chunked")
		return (true);
	return (false);
}

void Request::setBody(const std::string& body)
{
	this->_body = body;
}

std::string  Request::getBody() const
{
	return(this->_body);
}

/************************** UTILS **********************************/

/*
This function looks for the "boundary=" substring within the Content-Type header and
 returns the boundary value. If the boundary is quoted, it 
 properly extracts the quoted string.
 */
std::string extractBoundary(const std::string& contentType) {
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

bool	endsWithCRLF(const std::string& str) 
{
    if (str.size() < 3) 
		return false;
	std::cout << "last of line is " << str[str.size() - 1] << " and " << str[str.size() - 2] << std::endl;
    return (str[str.size() - 1] == '\r');
}


bool hasConsecutiveSpace(const std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
    {
        if (std::isspace(str[i]) && std::isspace(str[i + 1]))
            return true;
    }
    return false;
} 