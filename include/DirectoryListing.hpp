/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListing.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 12:06:49 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/25 12:56:06 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef DIRECTORYLISTING_HPP
# define DIRECTORYLISTING_HPP

# include <string>
# include <vector>
# include <dirent.h>
# include <iostream>

class DirectoryListing
{
private:
	std::string _path;

	DirectoryListing(const DirectoryListing& src);
	DirectoryListing& operator=(const DirectoryListing& src);
public:
	DirectoryListing(const std::string& path);
	~DirectoryListing();
	
	std::vector<std::string>	getListing(const std::string& path) const;
	std::string generateHTML(const std::string& file);
	std::string renderListing(const std::vector<std::string>& listing);
	std::string getHTMLListing();
};

#endif