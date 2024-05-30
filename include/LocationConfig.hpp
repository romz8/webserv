/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:08:51 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/30 14:17:51 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>

class ServerConfig;

struct CgiConfig 
{
    std::string extension;
    std::string handlerPath;
	CgiConfig(const std::string& ext, const std::string& handler)
    : extension(ext), handlerPath(handler) {}
};

class LocationConfig
{
private:
	std::map<std::string, bool > _isSet;
	std::string _uri;
    std::string _root;
    std::string _alias;
    std::map<int, std::string> _error_pages;
    std::vector<std::string> _allowed_methods;
	std::string _index;
	bool _autoIndex;
	bool _allowUpload;
	std::string _uploadDir;
	std::vector<CgiConfig> _cgiConfigs;
	std::vector<std::string>	_allowed_directives;

public:
	LocationConfig();
	LocationConfig( const ServerConfig &serv);
	LocationConfig(const LocationConfig &src);
	LocationConfig& operator=(const LocationConfig& src);
	~LocationConfig();

	void setUri(const std::string& uri);
	void setRootDir(const std::string& root);
	void setAlias(const std::string& alias);
	void setErrorPages(const std::map<int, std::string>& error_pages);
	void setAllowedMethods(const std::vector<std::string>& allowed_methods);
	void setIndex(const std::string& index);
	void setAutoIndex(bool autoIndex);
	void setAllowUpload(bool allowUpload);
	void setUploadDir(const std::string& uploadDir);

	void addErrorPage(int code, const std::string& page);
	void addCgiConfig(const CgiConfig& cgiConfig);

	bool isSet(std::string name);

	const std::string& getUri() const;
	const std::string& getRoot() const;
	const std::string& getAlias() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::vector<CgiConfig>& getCgiConfigs() const;
	const std::string& getIndex() const;
	bool getAutoIndex() const;
	bool getAllowUpload() const ;
	const std::string& getUploadDir() const;
	std::vector<std::string>	getAD(void);
};
#endif