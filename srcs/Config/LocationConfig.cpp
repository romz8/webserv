/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:24:39 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/21 18:41:49 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig(){}
LocationConfig::~LocationConfig(){}

LocationConfig::LocationConfig(const LocationConfig& src)
{
	*this = src;
}

LocationConfig& LocationConfig::operator=(const LocationConfig& src)
{
	if (this == &src)
		return (*this);
	_uri = src._uri;
	_root = src._root;
	_alias = src._alias;
	_error_pages = src._error_pages;
	_allowed_methods = src._allowed_methods;
	_cgiConfigs = src._cgiConfigs;
	_index = src._index;
	_autoIndex = src._autoIndex;
	_allowUpload = src._allowUpload;
	_uploadDir = src._uploadDir;
	return (*this);
}

void LocationConfig::setUri(const std::string& uri) { this->_uri = uri; }
void LocationConfig::setRoot(const std::string& root) { this->_root = root; }
void LocationConfig::setAlias(const std::string& alias) { this->_alias = alias; }
void LocationConfig::setErrorPages(const std::map<int, std::string>& error_pages) { this->_error_pages = error_pages; }
void LocationConfig::setAllowedMethods(const std::vector<std::string>& allowed_methods) { this->_allowed_methods = allowed_methods; }
void LocationConfig::addCgiConfig(const CgiConfig& cgiConfig) { _cgiConfigs.push_back(cgiConfig); }
void LocationConfig::setIndex(const std::string& index) { this->_index = index; }
void LocationConfig::setAutoIndex(bool autoIndex) { this->_autoIndex = autoIndex; }
void LocationConfig::setAllowUpload(bool allowUpload) { this->_allowUpload = allowUpload; }
void LocationConfig::setUploadDir(const std::string& uploadDir) { this->_uploadDir = uploadDir; }

const std::string& LocationConfig::getUri() const { return _uri; }
const std::string& LocationConfig::getRoot() const { return _root; }
const std::string& LocationConfig::getAlias() const { return _alias; }
const std::map<int, std::string>& LocationConfig::getErrorPages() const { return _error_pages; }
const std::vector<std::string>& LocationConfig::getAllowedMethods() const { return _allowed_methods; }
const std::vector<CgiConfig>& LocationConfig::getCgiConfigs() const { return _cgiConfigs; }
const std::string& LocationConfig::getIndex() const { return _index; }
bool LocationConfig::getAutoIndex() const { return _autoIndex; }
bool LocationConfig::getAllowUpload() const { return _allowUpload; }
const std::string& LocationConfig::getUploadDir() const { return _uploadDir; }

