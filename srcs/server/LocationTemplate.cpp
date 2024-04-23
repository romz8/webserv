/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationTemplate.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 21:09:16 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/23 22:33:34 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(){};

Location::Location(const std::string& path, const std::vector<std::string>& methods,
             const std::string& root, const std::string& index, bool autoIdx, const std::string& upload) : \
 _path(path), _allowedMethods(methods), _rootDir(root), _index(index), _AutoIndex(autoIdx), _uploadDirectory(upload)
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