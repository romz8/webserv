/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:57:04 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/23 22:21:30 by rjobert          ###   ########.fr       */
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
	std::string _path;
	std::string _rootDir;
	std::vector<std::string> _allowedMethods;
	std::string _uploadDirectory;
	std::string _index;
	bool _AutoIndex;      
	
public:
	Location();
	Location(const std::string& path, const std::vector<std::string>& methods,
             const std::string& root, const std::string& index, bool autoIdx, const std::string& uploadDir);
	~Location();
	bool isMethodAllowed(const std::string& method) const;
	bool match(const std::string& requestPath) const;
	std::string getPath() const;
	int getPrefixSize() const;
	std::string getRootDir() const;
	std::string getUploadFile() const;
};


#endif