/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romainjobert <romainjobert@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:57:04 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/16 17:18:41 by romainjober      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <iostream>
# include <vector>

class Location 
{
private:
	std::string _urlKey;
	std::string _path;
	std::vector<std::string> _methods;
	std::string _index;
public:
	Location(const std::string& url, const std::string& path) : _urlKey(url), _path(path){}
	~Location(){}
	bool match(const std::string& requestPath) const
	{
		return (requestPath.substr(0, this->_urlKey.size()) == this->_urlKey);
	}
	std::string getPath() const { return this->_path;}
	int getPrefixSize() const { return this->_urlKey.size();}
};


#endif