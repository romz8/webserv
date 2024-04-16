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
	std::string		_respBody;
	//std::string		_root; //will be used to remap the path to the root of the server
	std::map<std::string, std::string> _headers;

public:
	Request(const std::string& rawRequest, const std::string& hostName, int maxBody);
	~Request();
	Request(const Request& src);
	Request& operator=(const Request& src);

	void		parseHeader(const std::string& head);
	void		parseStartLine(const std::string& line);
	bool	isValidRL(const std::string& line);
	void	parseHeaderLine(const std::string& line);
	bool	endsWithCRLF(const std::string& str);
	bool 	hasConsecutiveSpace(const std::string& str);
	bool	hasCorrectHost() const;
	void		printRequest() const;
	void		parseBody(const std::string& header);
	void		parseChunkBody(const std::string& input);
	void		parseContentLenBody(const std::string& input);
	void		handlePostRequest();
	
	bool		isValidMethod() const;
	bool		isValidPath();
	bool		isValidVersion() const;
	void		sanitizeUrl();
	bool		isHiddenAccess(const std::string& url);
	
	void		setStatus();
	void		buildRequest();
	std::string	getMethod() const;
	std::string	getPath() const;
	int			getStatus() const;
	std::string getParsePath() const;
	std::string	getExtension() const;
	bool		isDirectory() const;
	
	void		parseExtension();
	bool		hasReadAccess() const;
	bool		typeCheck();
	void		normalizeDirPath();
	void		initRequest();
	
};

std::string trim(const std::string& str);

#endif