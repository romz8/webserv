/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:24:39 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/30 14:46:56 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <ServerConfig.hpp>
#include <ParseContent.hpp>
#include <Defines.hpp>
#include <Utils.hpp>

LocationConfig::LocationConfig() : _autoIndex(false), _allowUpload(false) {
	for (int i = 0; i < DIRECTIVES_NUM; i++)
		_isSet[ParseContent::total_directives[i]] = false;
	SUtils::set_allowed_directives(this->_allowed_directives, LOCATION);
}
LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig( const ServerConfig &serv ) {
	for (int i = 0; i < DIRECTIVES_NUM; i++)
		_isSet[ParseContent::total_directives[i]] = false;
	_root = serv.getRootDir();
	_error_pages = serv.getErrorPages();
	_autoIndex = serv.getAutoIndex();
	_allowUpload = false;
	SUtils::set_allowed_directives(this->_allowed_directives, LOCATION);
}

LocationConfig::LocationConfig(const LocationConfig& src)
{
	*this = src;
}

LocationConfig& LocationConfig::operator=(const LocationConfig& src)
{
	if (this != &src)
	{
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
		_isSet = src._isSet;
	}
	return (*this);
}

void LocationConfig::setUri(const std::string& uri) { this->_uri = uri; }
void LocationConfig::setRootDir(const std::string& root) { this->_root = root; _isSet["root"] = true;}
void LocationConfig::setAlias(const std::string& alias) { this->_alias = alias; _isSet["return"] = true;}
void LocationConfig::setErrorPages(const std::map<int, std::string>& error_pages) { this->_error_pages = error_pages; }
void LocationConfig::setAllowedMethods(const std::vector<std::string>& allowed_methods) { this->_allowed_methods = allowed_methods; _isSet["allowed_methods"] = true;}
void LocationConfig::addCgiConfig(const CgiConfig& cgiConfig) { _cgiConfigs.push_back(cgiConfig); _isSet["cgi"] = true;}
void LocationConfig::setIndex(const std::string& index) { this->_index = index; _isSet["index"] = true;}
void LocationConfig::setAutoIndex(bool autoIndex) { this->_autoIndex = autoIndex; _isSet["autoindex"] = true;}
void LocationConfig::setAllowUpload(bool allowUpload) { this->_allowUpload = allowUpload; _isSet["allow_upload"] = true;}
void LocationConfig::setUploadDir(const std::string& uploadDir) { this->_uploadDir = uploadDir; _isSet["upload_dir"] = true;}

void LocationConfig::addErrorPage(int code, const std::string& page) {
	_error_pages[code] = page;
	_isSet["error_pages"] = true;
}

bool LocationConfig::isSet( std::string name ) {
	return (_isSet[name]);
}

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
std::vector<std::string>	LocationConfig::getAD( void ) { return (this->_allowed_directives); };


