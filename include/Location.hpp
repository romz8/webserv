/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:57:04 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/11 21:48:06 by rjobert          ###   ########.fr       */
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