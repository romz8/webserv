/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:55:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/04 23:17:57 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

/* Constructor: Initializes the header object by parsing the provided raw HTTP 
header string. It sets up initial request parameters and catches any exceptions 
during parsing, setting an error status if needed.
*/
Header::Header(const std::string& rawHeader)
{
	initRequest();
	try
	{
		parseHeader(rawHeader);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		this->_status = 400;
	}
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
first we parse the start line (containing the method, path and version)
then we parse the headers as key value pairs inside a map
we implement a trimming for key, value for insrting in the map
*/
void Header::parseHeader(const std::string& head)
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
void Header::parseStartLine(const std::string& line)
{
	std::istringstream lineStream(line);
	lineStream >> this->_method >> this->_path >> this->_version;
}

void	Header::parseHeaderLine(const std::string& line)
{
	if (line.empty())
		return;
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		throw std::runtime_error("Error parsing header : no colon (:) at line " + line);
	std::string key = trim(line.substr(0, pos));
	std::string value = trim(line.substr(pos + 1));
	this->_headers[key] = value;
}

/*
Parses a single header line, extracting the header name and value, and storing them after trimming whitespace.
Skip empty lines to prevent parsing errors, then Locate the delimiter between header name and value.
If no delimiter is found, the header line is malformed. We then extract and trim the header name and value, 
storing them in the header map. Finally Store the key-value pair in the header map
*/ 
void Header::printHeader() const
{
	std::cout << YELLOW "Method : " << this->_method << std::endl;
	std::cout << "status is : " << this->_status << std::endl;
	std::cout << "Path : " << this->_path << std::endl;
	std::cout << "Version : " << this->_version << std::endl;
	std::cout << "Extension is  : " << this->_extension << std::endl;
	std::cout << "is Dir  : " << this->_isDirectory << std::endl;
	std::cout << "is DirNorm  : " << this->_isDirNorm << std::endl;
	std::cout << "Headers : " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		std::cout << it->first << " :" << it->second << std::endl;
	}
	std::cout << RESET << std::endl;
}

void	Header::initRequest()
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
Core of the machin after header parsing
*/
void	Header::buildRequest()
{
	// this->_path = sanitizeUrl(_path);
	setStatus();
	parseExtension();
	std::cout << YELLOW "Request status-Line is : " << this->_method << " " << this->_path << " " << this->_version << RESET << std::endl;
}

/*
Analyzes the HTTP request details stored in the Header object to determine the appropriate
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
void	Header::setStatus()
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

bool Header::isValidMethod() const
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
bool Header::isValidPath() 
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
void Header::normalizeDirPath()
{
	std::string path = this->_parsePath;
	if (path[path.size() - 1] == '/')
		return ;
	this->_isDirNorm = true;
	this->_parsePath = this->_path.append("/");
	return ;
}



bool Header::isValidVersion() const
{
	if (this->_version == "HTTP/1.1")
		return (true);
	return (false);
}

std::string Header::getMethod() const
{
	return (this->_method);
}

std::string Header::getPath() const
{
	return (this->_path);
}

std::string Header::getParsePath() const
{
	return (this->_parsePath);
}

std::string Header::getExtension() const
{
	return (this->_extension);
}

int Header::getStatus() const
{
	return (this->_status);
}

// R_OK tests for readability only
bool Header::hasReadAccess() const
{	
    if (access(this->_parsePath.c_str(), R_OK) == 0)
		return(true);
	return (false);
}

void	Header::parseExtension()
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
Remove occurrences of '/../' to prevent directory traversal
Change '//' to '/' to avoid a url to go up the env to filesys
Block access to hidden files and directories (any /. (like /.env) -> redirect to root)
*/
std::string Header::sanitizeUrl(const std::string& urlSrc)
{
	std::string url = urlSrc;
	size_t pos;
	while ((pos = url.find("/../") != std::string::npos))
		url.erase(pos, 3);
	
	while ((pos = url.find("//") != std::string::npos))
	{
		url.replace(pos, 2, "/");
		pos += 1;
	}

	// if ((pos = url.find("/.")) != std::string::npos) -- to ameliorate to avoid conflict wiht /../
	// 	url = "/";
	return (url);	
}

std::string trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}
