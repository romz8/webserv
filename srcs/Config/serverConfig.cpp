/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:24:47 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/30 05:24:37 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <ParseContent.hpp>
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
		client_max_body_size = src.client_max_body_size;
		cgiConf = src.cgiConf;
		autoindex = src.autoindex;
	}
	return (*this);
}

ServerConfig::ServerConfig(const std::string& host, int port, const std::string& serverName, const std::string& rootDir, const std::vector<LocationConfig>& locations, const std::map<int, std::string>& error_pages, size_t client_max_body_size, const std::vector<CgiConfig>& cgiConf, bool autoindex)	
	: _host(host), _port(port), _serverName(serverName), _rootDir(rootDir), _locations(locations), error_pages(error_pages), client_max_body_size(client_max_body_size), cgiConf(cgiConf), autoindex(autoindex)
{
}

void	ServerConfig::_initConfig()
{
	_host = "";
	_hostName = "";
	_port = 0;
	_serverName = "";
	_rootDir = "";
	_locations.clear();
	error_pages.clear();
	client_max_body_size = DEFAULT_BODY_SIZE;
	cgiConf.clear();
	autoindex = false;
}

ServerConfig::~ServerConfig() {}

std::string ServerConfig::getHost() const {return (_host);}

int ServerConfig::getPort() const {return (_port);}

std::string ServerConfig::getServerName() const {return (_serverName);}

std::string ServerConfig::getRootDir() const {return (_rootDir);}

std::vector<LocationConfig> ServerConfig::getLocationConf() const {return (_locations);}

std::map<int, std::string> ServerConfig::getErrorPages() const {return (error_pages);}

size_t ServerConfig::getClientMaxBodySize() const {return (client_max_body_size);}

std::vector<CgiConfig> ServerConfig::getCgiConf() const {return (cgiConf);}

bool ServerConfig::getAutoIndex() const {return (autoindex);}

void ServerConfig::setHost(const std::string& host) {_host = host;}

void ServerConfig::setPort(int port) {_port = port;}

void ServerConfig::setServerName(const std::string& serverName) {_serverName = serverName;}

void ServerConfig::setRootDir(const std::string& rootDir) {_rootDir = rootDir;}

void ServerConfig::addLocationConfig(const LocationConfig& locations) 
{
	_locations.push_back(locations);
}

// void ServerCOnfig::deleteErrorPageWithKey( int key)

void ServerConfig::addErrorPage(int code, const std::string& page)
{
	error_pages[code] = page;
}

void ServerConfig::setErrorPages(const std::map<int, std::string>& error_pages)
{
	this->error_pages = error_pages;
}

void ServerConfig::setClientMaxBodySize(size_t client_max_body_size)
{
	if (client_max_body_size > MAX_BODY_SIZE)
		this->client_max_body_size = MAX_BODY_SIZE - 1;
	else
		this->client_max_body_size = client_max_body_size;
}

void ServerConfig::addCgiConfig(const CgiConfig& cgiConf)
{
	this->cgiConf.push_back(cgiConf);
}
void ServerConfig::setCgiConf(const std::vector<CgiConfig>& cgiConf)
{
	this->cgiConf = cgiConf;
}

void ServerConfig::setAutoIndex(bool autoindex)
{
	this->autoindex = autoindex;
}

void ServerConfig::setHostName(const std::string& hostName)
{
	_hostName = hostName;
}

std::string ServerConfig::getHostName() const
{
	return (_hostName);
}


std::vector<std::string>	ServerConfig::getAD( void ) const {
	return (this->_allowed_directives);
}

std::vector<LocationConfig> &ServerConfig::getLocationConfEdit() {return (_locations); }
