/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:44:52 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/03 20:04:59 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include <string>
# include <vector>
# include <map>
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
	int				_status;
	std::string		_respBody;
	//std::string		_root; //will be used to remap the path to the root of the server
	std::map<std::string, std::string> _headers;

public:
	Header(const std::string& rawHeader);
	~Header();
	Header(const Header& src);
	Header& operator=(const Header& src);

	void	parseHeader(const std::string& head);
	void	printHeader() const;
	bool	isValidMethod() const;
	bool	isValidPath();
	bool	isValidVersion() const;
	void	buildRequest();
	std::string	getMethod() const;
	std::string	getPath() const;
	int			getStatus() const;
	int			fileExists() const;
	bool		isDirectory() const;
	std::string getParsePath() const;
	bool		hasReadAccess() const;
	bool typeCheck();
	void	normalizeDirPath();
	void	initRequest();
};

#endif