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

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>
# include <unistd.h>
# include <dirent.h>
# include <ctime>
# include <cstdlib>
# include "colors.h"
# include "Server.hpp"
# include "Location.hpp"
# include "ServerConfig.hpp"
# include "DirectoryListing.hpp"

typedef struct s_cgi
{
	std::map<std::string, std::string>	_env;
	std::string							_path;
	std::string							_body;
	std::string							_respbody;
	std::string							_execPath;
	int		_fdin[2];
	int		_fdout[2];
	pid_t _pid;
	time_t _start;
	bool _onCGI;
	
} t_cgi;

class Request
{
private:
	std::string		_host;
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
	std::string		_respbody;
	Location		_location;
	bool 			_execCgi;
	std::string		_query;
	std::map<std::string, std::string> _headers;
	std::vector<std::string>		_serverName;
	int				_port;
	bool			_HeaderRead;
	bool			_HeaderOK;
	std::string		_rawinput;
	std::string		_rawBody;
	t_cgi			_cgi;
	

	time_t			_start;
	static const int _timeout = 5;
	//static const std::string CRLF = "\r\n";

public:
	Request(const std::string host, const int maxBody, const std::vector<std::string> servName, const int port);
	//Request();
	~Request();
	Request(const Request& src);
	Request& operator=(const Request& src);

	void		initRequest();
	bool		_readRequest(char* buffer, int byteSize);
	bool		processHeader(const std::string& rawHead);
	void		parseHeader(const std::string& head);
	void		parseStartLine(const std::string& line);
	bool		isValidRL(const std::string& line);
	void		parseHeaderLine(const std::string& line);
	bool		hasCorrectHost() const;
	bool		hasBody();
	void		printRequest() const;
	void		printHeader() const;
	bool		parseBody();
	//bool		parseBody(const std::string& header);
	void		parseChunkBody(const std::string& input);
	bool		parseContentLenBody();
	void		byteUpload(char *buffer, int byteSize);

	void		processMultipartForm(const std::string& input, const std::string& boundary);
	void		processFormData(const std::string& body, const Location& Loc);
	bool		processChunkBody(std::string buffer);
	void		handlePostRequest();
	void		handleDeleteRequest();
	void		handleGetRequest();
	bool		handleCgi();
	void		DeleteDirectory();
	void		StatusCode();
	void		buildRequest();
	void		initCgi(std::string execPath);
	void		executeCGI();
	void		checkCGISHeader();

	bool		isValidMethod() const;
	bool		isValidPath();
	bool		isValidVersion() const;
	bool		isDirectory() const;
	void		sanitizeUrl();
	bool		isHiddenAccess(const std::string& url);
	bool		hasReadAccess() const;
	bool		hasWriteAccess() const;
	bool		hasExecAccess() const;
	void		normalizeDirPath();
	bool		execCgi() const;

	std::string	getMethod() const;
	std::string	getPath() const;
	int			getStatus() const;
	std::string getParsePath() const;
	std::string getQueryString() const;
	std::string	getExtension() const;
	std::map<std::string, std::string> getHeader() const;
	std::string getHeaderField(const std::string& key) const;
	std::string getBody() const;
	std::string getrespBody() const;
	void		setBody(const std::string& body);
	void		setStatus(int status);
	void		setLocation(const Location& loc);
	Location	getLocation() const;
	void		getQueryParams();
	void		setServerName(const std::string& serverName);
	void		setPort(int port);
	std::string getServerName() const;
	int			getPort() const;
	void		setHost(const std::string& host);
	std::string getHost() const;
	void		setPath(const std::string& path);
	std::string getrawBody() const;
	t_cgi&		getCgi();
	friend std::ostream& operator<<(std::ostream& os, const Request& req);
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
bool	loneCR(const std::string& header);
std::string	formattedTime();
void	hexDecoding(std::string& url);
std::ostream& operator<<(std::ostream& os, const Request& req);
bool isExecutable(const std::string& path);
bool foundStringinVec(const std::string& target, const std::vector<std::string>& vec);

#endif