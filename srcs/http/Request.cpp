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
Request::Request(const std::string& rawRequest)
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
		this->_Requests = src._Requests;
	}
	return (*this);
}

/*
first we parse the start line (containing the method, path and version)
then we parse the Requests as key value pairs inside a map
we implement a trimming for key, value for insrting in the map
*/
void Request::parseHeader(const std::string& head)
{
	std::istringstream stream(head);
	std::string line;
	
	if (std::getline(stream, line))
		parseStartLine(line);
	else
		throw std::runtime_error("Empty startline");
	try
	{
		while (std::getline(stream, line) && !(line == "\r"))
			parseHeaderLine(line);
	}
	catch(const std::exception& e)
	{
		throw;
	}
}

// Extracts the HTTP method, path, and version from the start line of the request.
void Request::parseStartLine(const std::string& line)
{
	std::istringstream lineStream(line);
	lineStream >> this->_method >> this->_path >> this->_version;
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
	this->_Requests[key] = value;
}

/*
Parses a single Request line, extracting the Request name and value, and storing them after trimming whitespace.
Skip empty lines to prevent parsing errors, then Locate the delimiter between Request name and value.
If no delimiter is found, the Request line is malformed. We then extract and trim the Request name and value, 
storing them in the Request map. Finally Store the key-value pair in the Request map
*/ 
void Request::printRequest() const
{
	std::cout << YELLOW "Method : " << this->_method << std::endl;
	std::cout << "status is : " << this->_status << std::endl;
	std::cout << "Path : " << this->_path << std::endl;
	std::cout << "Version : " << this->_version << std::endl;
	std::cout << "Extension is  : " << this->_extension << std::endl;
	std::cout << "is Dir  : " << this->_isDirectory << std::endl;
	std::cout << "is DirNorm  : " << this->_isDirNorm << std::endl;
	std::cout << "Requests : " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_Requests.begin(); it != this->_Requests.end(); ++it)
	{
		std::cout << it->first << " :" << it->second << std::endl;
	}
	std::cout << RESET << std::endl;
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
	this->_respBody.clear();
	this->_extension.clear();
}
/*
Core of the machin after Request parsing
*/
void	Request::buildRequest()
{
	std::cout << BG_BLUE "INIT _PATH : " << this->_path << RESET << std::endl;
	sanitizeUrl();
	setStatus();
	parseExtension();
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

/*
verify that path is ok at first (not empty, starting with /)
then check if the path is either a file or a dir (with stat())
if neither -> raiseError by returning null, otherwise update Response State
*/
bool Request::isValidPath() 
{
	std::string testpath = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html"; //to be cahnge for dynamic root and env var
	struct stat path_stat;
	
	if (this->_path.empty() || this->_path[0] != '/')
		return (false);
	this->_parsePath = testpath.append(this->_path);
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



bool Request::isValidVersion() const
{
	if (this->_version == "HTTP/1.1")
		return (true);
	return (false);
}

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

// R_OK tests for readability only
bool Request::hasReadAccess() const
{	
    if (access(this->_parsePath.c_str(), R_OK) == 0)
		return(true);
	return (false);
}

void	Request::parseExtension()
{
	if (this->_parsePath.empty() || this->_status != 200)
	{
		this->_extension = ".html";
		return ;
	}
	size_t pos = _parsePath.find_last_of('.');
	if (pos != std::string::npos)
    	this->_extension = _parsePath.substr(pos);
	else
		this->_extension = ".html"; // chose the logic to hanle extension parsing error - especially for directories
	std::cout << RED "EXT IS : " << this->_extension << RESET << std::endl;
}
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
