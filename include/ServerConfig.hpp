/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:03:12 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/22 14:36:37 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include "LocationConfig.hpp"

# define MAX_BODY_SIZE 10000000000 //10GB
# define DEFAULT_BODY_SIZE 10000000 //10MB
class ServerConfig
{
private:
	std::string _host;
	std::string _hostName;
	int _port;
	std::string _serverName;
	std::string _rootDir;
	std::vector<LocationConfig> _locations;
	std::map<int, std::string> error_pages;
    size_t _max_body_size;
	std::vector<CgiConfig> cgiConf;
    bool autoindex;

public:	
	ServerConfig();
	ServerConfig& operator=(const ServerConfig& src);
	ServerConfig(const ServerConfig& src);
	ServerConfig(const std::string& host, int port, const std::string& serverName, const std::string& rootDir, const std::vector<LocationConfig>& locations, const std::map<int, std::string>& error_pages, size_t client_max_body_size, const std::vector<CgiConfig>& cgiConf, bool autoindex);
	~ServerConfig();
	
	void _initConfig();
	std::string getHost() const;
	int getPort() const;
	std::string getServerName() const;
	std::string getRootDir() const;
	std::vector<LocationConfig> getLocationConf() const;
	std::map<int, std::string> getErrorPages() const;
	size_t getMaxBodySize() const;
	std::vector<CgiConfig> getCgiConf() const;
	bool getAutoIndex() const;
	void setHost(const std::string& host);
	void setPort(int port);
	void setServerName(const std::string& serverName);
	void setRootDir(const std::string& rootDir);
	void addLocationConfig(const LocationConfig& locations);
	void addErrorPage(int code, const std::string& page);
	void setErrorPages(const std::map<int, std::string>& error_pages);
	void setClientMaxBodySize(size_t client_max_body_size);
	void setCgiConf(const std::vector<CgiConfig>& cgiConf);
	void addCgiConfig(const CgiConfig& cgiConf);
	void setAutoIndex(bool autoindex);
	void setHostName(const std::string& hostName);
	std::string getHostName() const;
};

#endif