/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:24:47 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/30 17:32:11 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <ParseContent.hpp>
#include <Defines.hpp>
#include <Utils.hpp>

ServerConfig::ServerConfig()
{
	SUtils::set_allowed_directives(this->_allowed_directives, SERVER);
	_initConfig();
}

ServerConfig::ServerConfig(const ServerConfig& src)
{
	*this = src;
}	

ServerConfig& ServerConfig::operator=(const ServerConfig& src)
{
	if (this != &src)
	{
		_allowed_directives = src._allowed_directives;
		_host = src._host;
		_hostName = src._hostName;
		_port = src._port;
		_serverName = src._serverName;
		_rootDir = src._rootDir;
		_locations = src._locations;
		error_pages = src.error_pages;
		_max_body_size = src._max_body_size;
		cgiConf = src.cgiConf;
		autoindex = src.autoindex;
	}
	return (*this);
}

// ServerConfig::ServerConfig(const std::string& host, int port, const std::string& serverName, const std::string& rootDir, const std::vector<LocationConfig>& locations, const std::map<int, std::string>& error_pages, size_t client_max_body_size, const std::vector<CgiConfig>& cgiConf, bool autoindex)	
// 	: _host(host), _port(port), _rootDir(rootDir), _locations(locations), error_pages(error_pages), _max_body_size(client_max_body_size), cgiConf(cgiConf), autoindex(autoindex)
// {
	
// }

void	ServerConfig::_initConfig()
{
	for (int i = 0; i < DIRECTIVES_NUM; i++)
		_isSet[ParseContent::total_directives[i]] = false;
	_host = "";
	_hostName = "";
	_port = 0;
	_serverName.clear();
	_rootDir = DEFAULTROOOT;
	_locations.clear();
	error_pages.clear();
	_max_body_size = DEFAULT_BODY_SIZE;
	cgiConf.clear();
	autoindex = false;
}

ServerConfig::~ServerConfig() {}

std::string ServerConfig::getHost() const {return (_host);}

int ServerConfig::getPort() const {return (_port);}

std::vector<std::string> ServerConfig::getServerName() const {return (_serverName);}

std::string ServerConfig::getRootDir() const {return (_rootDir);}

std::vector<LocationConfig> ServerConfig::getLocationConf() const {return (_locations);}

std::map<int, std::string> ServerConfig::getErrorPages() const {return (error_pages);}

size_t ServerConfig::getMaxBodySize() const {return (_max_body_size);}

std::vector<CgiConfig> ServerConfig::getCgiConf() const {return (cgiConf);}

bool ServerConfig::getAutoIndex() const {return (autoindex);}

void ServerConfig::setHost(const std::string& host) {_host = host;}

void ServerConfig::setPort(int port) {_port = port;}

void ServerConfig::setServerName(std::string serverName) {_serverName.push_back(serverName);}

void ServerConfig::setRootDir(const std::string& rootDir) {_rootDir = rootDir;}

void ServerConfig::addLocationConfig(const LocationConfig& locations) 
{
	_locations.push_back(locations);
	_isSet["location"] = true;
}

bool ServerConfig::isSet( std::string name ) {
	return (_isSet[name]);
}

void ServerConfig::addErrorPage(int code, const std::string& page)
{
	error_pages[code] = page;
	_isSet["error_pages"] = true;
}

void ServerConfig::setErrorPages(const std::map<int, std::string>& error_pages)
{
	this->error_pages = error_pages;
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
	if (size > MAX_BODY_SIZE)
		this->_max_body_size = MAX_BODY_SIZE - 1;
	else
		this->_max_body_size = size;
	_isSet["client_max_body_size"] = true;
	
}

void ServerConfig::addCgiConfig(const CgiConfig& cgiConf)
{
	this->cgiConf.push_back(cgiConf);
	_isSet["cgi"] = true;
}
void ServerConfig::setCgiConf(const std::vector<CgiConfig>& cgiConf)
{
	this->cgiConf = cgiConf;
}

void ServerConfig::setAutoIndex(bool autoindex)
{
	this->autoindex = autoindex;
	_isSet["autoindex"] = true;
}

void ServerConfig::setHostName(const std::string& hostName)
{
	_hostName = hostName;
}

std::string ServerConfig::getHostName() const
{
	return (_hostName);
}

void	ServerConfig::setListen(int port, std::string ip) {
	setPort(port);
	setHostName(ip);
	_isSet["listen"] = true;
}

std::vector<std::string>	ServerConfig::getAD( void ) const {
	return (this->_allowed_directives);
}

std::vector<LocationConfig> &ServerConfig::getLocationConfEdit() {return (_locations); }
