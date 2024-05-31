/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 21:09:16 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/31 08:03:25 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <Utils.hpp>

Location::Location() : _AutoIndex(true), _allowUpload(true) {};

Location::Location(const LocationConfig& locConf) : \
	_path(locConf.getUri()), _rootDir(locConf.getRootDir()), _allowedMethods(locConf.getAllowedMethods()), \
	_uploadDirectory(locConf.getUploadDir()), _index(locConf.getIndex()), _AutoIndex(locConf.getAutoIndex()), \
	_allowUpload(locConf.getAllowUpload())
{
	_cgiConfigs.clear();
	_errPages.clear();
	_cgiConfigs = locConf.getCgiConfigs();
	_errPages = locConf.getErrorPages();
	_alias = locConf.getAlias();
	std::cout << "Location object created with path : " << this->_path << " and root : " << this->_rootDir << std::endl;
	//std::cout << *this << std::endl;
}
/*
Location::Location(const std::string& path, const std::vector<std::string>& methods,
             const std::string& root, const std::string& index, bool autoIdx, bool allowup, const std::string& upload) : \
 _path(path), _allowedMethods(methods), _rootDir(root), _index(index), _AutoIndex(autoIdx), \
 _allowUpload(allowup), _uploadDirectory(upload)
{
	_cgiConfigs.clear();
	//std::cout << "Location object created with path : " << this->_path << " and root : " << this->_rootDir << std::endl;
	//std::cout << " and upload path is " << this->_uploadDirectory << std::endl;
}*/
Location::~Location(){}

Location::Location(const Location& src)
{
	*this = src;
}

Location& Location::operator=(const Location& src)
{
	if (this == &src)
		return (*this);
	_path = src._path;
	_alias = src._alias;
	_rootDir = src._rootDir;
	_allowedMethods = src._allowedMethods;
	_uploadDirectory = src._uploadDirectory;
	_index = src._index;
	_AutoIndex = src._AutoIndex;
	_allowUpload = src._allowUpload;
	_errPages = src._errPages;
	_cgiConfigs = src._cgiConfigs;
	return (*this);
}

bool Location::isMethodAllowed(const std::string& method) const 
{
	return (SUtils::it_easyfind< std::vector<std::string> >(_allowedMethods, method) != _allowedMethods.end());
}

std::string Location::getRootDir() const
{
	return (this->_rootDir);
}

std::string Location::getPath() const
{
	return (this->_path);
}

std::string Location::getUploadFile() const
{
	return (this->_uploadDirectory);
}

std::string Location::getIndex() const
{
	return (this->_index);
}	

void	Location::setIndex(const std::string& index)
{
	this->_index = index;
}

void Location::addErrPage(const std::map<int, std::string>& srcPages)
{
	std::map<int, std::string>::const_iterator it = srcPages.begin();
	for (; it != srcPages.end(); ++it)
		this->_errPages[it->first] = it->second;
}

std::string Location::getErrPage(int code) const
{
	std::map<int, std::string>::const_iterator it = this->_errPages.find(code);
	if (it != this->_errPages.end())
		return (it->second);
	return ("");
}

bool Location::getAutoIndex() const
{
	return (this->_AutoIndex);
}

bool Location::getUploadAllowed() const
{
	return (this->_allowUpload);
}

void Location::setCgi(const CgiConfig& cgi)
{
	_cgiConfigs.push_back(cgi);
}

// void Location::setCgi(const std::map<std::string, const std::string>& cgiMap)
// {
// 	_cgiConfigs.push_back({ext, cgiMap.});
// }

std::vector<CgiConfig> Location::getCgi() const
{
	return (this->_cgiConfigs);
}

bool Location::hasCgi(const std::string& extension) const 
{
    for (std::vector<CgiConfig>::const_iterator it = _cgiConfigs.begin(); it != _cgiConfigs.end(); ++it) 
	{
		if (it->extension == extension)
            return true;
    }
    return false;
}

std::string Location::getCgiHandler(const std::string& extension) const 
{
    for (size_t i = 0; i < _cgiConfigs.size(); ++i) {
        if (_cgiConfigs[i].extension == extension) {
            return (_cgiConfigs[i].handlerPath);
        }
    }
    return "";  // Return an empty string if no handler is found
}

std::string Location::getAlias() const
{
	return (this->_alias);
}

void Location::setAlias(const std::string& alias)
{
	this->_alias = alias;
}

std::ostream& operator<<(std::ostream& os, const Location& loc)
{
	std::cout << "**** Location Info *****" << std::endl;
	std::cout << "Path : " << loc._path << std::endl;
	std::cout << "Root Directory : " << loc._rootDir << std::endl;
	std::cout << "Allowed Methods : ";
	for (std::vector<std::string>::const_iterator it = loc._allowedMethods.begin(); it != loc._allowedMethods.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
	std::cout << "Upload Directory : " << loc._uploadDirectory << std::endl;
	std::cout << "Index : " << loc._index << std::endl;
	std::cout << "AutoIndex : " << loc._AutoIndex << std::endl;
	std::cout << "Allow Upload : " << loc._allowUpload << std::endl;
	std::cout << "Alias : " << loc._alias << std::endl;
	std::cout << "Error Pages : " << std::endl;
	for (std::map<int, std::string>::const_iterator it = loc._errPages.begin(); it != loc._errPages.end(); ++it)
		std::cout << "Error Code : " << it->first << " Page : " << it->second << std::endl;
	std::cout << "Cgi Configs : " << std::endl;
	for (std::vector<CgiConfig>::const_iterator it = loc._cgiConfigs.begin(); it != loc._cgiConfigs.end(); ++it)
		std::cout << "Extension : " << it->extension << " Handler : " << it->handlerPath << std::endl;
	return (os);
}