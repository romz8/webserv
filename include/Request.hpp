/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:44:52 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/05 10:18:36 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Request_HPP
# define Request_HPP

# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include "colors.h"
# include "Location.hpp"

class Request
{
private:
	std::string		_hostName;
	std::string		_method;
	std::string		_path;
	std::string		_version;
	bool			_isDirectory;
	bool			_isDirNorm;
	std::string		_parsePath;
	std::string		_extension;
	int				_status;
	int				_maxBodySize;
	std::string		_chunkBody;
	std::string		_body;
	Location		_location;
	std::map<std::string, std::string> _headers;

public:
	Request(const std::string& rawRequest, const std::string& hostName, int maxBody);
	~Request();
	Request(const Request& src);
	Request& operator=(const Request& src);

	void		parseHeader(const std::string& head);
	void		parseStartLine(const std::string& line);
	bool		isValidRL(const std::string& line);
	void		parseHeaderLine(const std::string& line);
	bool		hasCorrectHost() const;
	bool		hasBody();
	void		printRequest() const;
	void		printHeader() const;
	void		parseBody(const std::string& header);
	void		parseChunkBody(const std::string& input);
	bool		parseContentLenBody();
	void		processMultipartForm(const std::string& input, const std::string& boundary);
	void		processFormData(const std::string& body, const Location& Loc);
	void		processChunkBody(std::string buffer);
	void		handlePostRequest();
	void		handleDeleteRequest();
	void		handleGetRequest();
	void		DeleteDirectory();
	
	bool		isValidMethod() const;
	bool		isValidPath();
	bool		isValidVersion() const;
	void		sanitizeUrl();
	bool		isHiddenAccess(const std::string& url);
	
	void		StatusCode();
	void		buildRequest();
	std::string	getMethod() const;
	std::string	getPath() const;
	int			getStatus() const;
	std::string getParsePath() const;
	std::string	getExtension() const;
	std::map<std::string, std::string> getHeader() const;
	std::string getBody() const;
	void		setBody(const std::string& body);
	void		setStatus(int status);
	void		setLocation(const Location& loc);
	Location	getLocation() const;
	bool		isDirectory() const;
	
	bool		hasReadAccess() const;
	bool		hasWriteAccess() const;
	void		normalizeDirPath();
	void		initRequest();
	
}; 

/********************* utils ******************************/
std::string parseExtension(const std::string& path, const std::string& def);
std::string trim(const std::string& str);
std::string extractBoundary(const std::string& contentType);
bool	endsWithCRLF(const std::string& str);
bool 	hasConsecutiveSpace(const std::string& str);
bool	deleteResource(const std::string& path);
bool	fileExists(const std::string& path);
size_t safeStrToSizeT(const std::string& str);

#endif