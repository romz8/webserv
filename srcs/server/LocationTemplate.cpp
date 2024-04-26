/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationTemplate.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 21:09:16 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/24 21:49:38 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(){};

Location::Location(const std::string& path, const std::vector<std::string>& methods,
             const std::string& root, const std::string& index, bool autoIdx, bool allowup, const std::string& upload) : \
 _path(path), _allowedMethods(methods), _rootDir(root), _index(index), _AutoIndex(autoIdx), \
 _allowUpload(allowup), _uploadDirectory(upload)
{
	std::cout << "Location object created with path : " << this->_path << " and root : " << this->_rootDir << std::endl;
	std::cout << " and upload path is " << this->_uploadDirectory << std::endl;
}
Location::~Location(){}

bool Location::isMethodAllowed(const std::string& method) const 
{
    return std::find(_allowedMethods.begin(), _allowedMethods.end(), method) != _allowedMethods.end();
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