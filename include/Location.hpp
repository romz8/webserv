/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:57:04 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/29 18:25:51 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include "LocationConfig.hpp"


class Location 
{
private:
	std::string _path;
	std::string _rootDir;
	std::vector<std::string> _allowedMethods;
	std::string _uploadDirectory;
	std::string _index;
	bool _AutoIndex;
	bool _allowUpload;
	std::string _alias;
	std::map<int, std::string> _errPages;
	std::vector<CgiConfig> _cgiConfigs;
	
public:
	Location();
	Location(const LocationConfig& locConf);
	Location(const Location& src);
	Location& operator=(const Location& src);
	~Location();
	bool isMethodAllowed(const std::string& method) const;
	std::string getPath() const;
	std::string getRootDir() const;
	std::string getUploadFile() const;
	std::string getIndex() const;
	void setIndex(const std::string& index);
	void addErrPage(const std::map<int, std::string>& srcPages);
	std::string getErrPage(int code) const;
	bool getAutoIndex() const;
	bool getUploadAllowed() const;
	std::vector<CgiConfig> getCgi() const;
	void setCgi(const CgiConfig& cgi);
	bool hasCgi(const std::string& extension) const;
	std::string	getCgiHandler(const std::string& extension) const;
	std::string getAlias() const;
	void setAlias(const std::string& alias);
	
	friend std::ostream& operator<<(std::ostream& os, const Location& loc);
	
};

std::ostream& operator<<(std::ostream& os, const Location& loc);


#endif