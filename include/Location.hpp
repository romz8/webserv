/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:57:04 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/24 10:31:15 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include "LocationConfig.hpp"

// struct CgiConfig 
// {
//     std::string extension;
//     std::string handlerPath;
// 	CgiConfig(const std::string& ext, const std::string& handler)
//     : extension(ext), handlerPath(handler) {}
// };

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
	// Location(const std::string& path, const std::vector<std::string>& methods,
    //          const std::string& root, const std::string& index, bool autoIdx, bool allowupload, \
	// 		 const std::string& uploadDir); //should we add a map of error pages?
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
	//void setCgi(const std::map<std::string, const std::string>& cgiMap);
	bool hasCgi(const std::string& extension) const;
	std::string	getCgiHandler(const std::string& extension) const;
	std::string getAlias() const;
	void setAlias(const std::string& alias);
	
	friend std::ostream& operator<<(std::ostream& os, const Location& loc);
	
};

std::ostream& operator<<(std::ostream& os, const Location& loc);


#endif