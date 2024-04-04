/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:44:52 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/04 22:48:38 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

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

class Header
{
private:
	std::string		_method;
	std::string		_path;
	std::string		_version;
	bool			_isDirectory;
	bool			_isDirNorm;
	std::string		_parsePath;
	std::string		_extension;
	int				_status;
	std::string		_respBody;
	//std::string		_root; //will be used to remap the path to the root of the server
	std::map<std::string, std::string> _headers;

public:
	Header(const std::string& rawHeader);
	~Header();
	Header(const Header& src);
	Header& operator=(const Header& src);

	void		parseHeader(const std::string& head);
	void		parseStartLine(const std::string& line);
	void	parseHeaderLine(const std::string& line);
	void		printHeader() const;
	
	bool		isValidMethod() const;
	bool		isValidPath();
	bool		isValidVersion() const;
	std::string	sanitizeUrl(const std::string& urlSrc);
	
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