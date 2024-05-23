/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListing.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 11:53:03 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/26 10:36:49 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryListing.hpp"

DirectoryListing::DirectoryListing(const std::string& path) : _path(path){}

DirectoryListing::~DirectoryListing(){}

std::vector<std::string> DirectoryListing::getListing(const std::string& path) const
{
	std::vector<std::string> listing;
	std::string tmp;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		int count = 0;
		while ((ent = readdir(dir)) != NULL)
		{
			tmp = std::string(ent->d_name);
			if (tmp != "." && tmp != "..")
				listing.push_back(tmp);
		}
		closedir(dir);
		if (listing.size() == 0)
			listing.push_back("Empty directory");
	}
	else
		listing.push_back("Error opening directory");
	return (listing);
}

std::string DirectoryListing::generateHTML(const std::string& file)
{
	std::string html;

	html = "<li><a href=\"" + file + "\">" + file + "</a></li>";
	std::cout << "HTML : " << html << std::endl;
	return (html);
}

std::string DirectoryListing::renderListing(const std::vector<std::string>& listing)
{
	std::string html;
	html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Listing</title>\n</head>\n<body>\n";
	html += "<h1>Directory Listing</h1>\n";
	if (listing.size() == 1 && (listing[0] == "Empty directory" || listing[0] == "Error opening directory"))
	{
		
		html += "<p>" + listing[0] + "</p>\n</body>\n</html>\n";
		return (html);
	}
	html += "<ul>\n";
	for (std::vector<std::string>::const_iterator it = listing.begin(); it != listing.end(); ++it)
		html += generateHTML(*it);
	html += "</ul>\n</body>\n</html>\n";
	return (html);
}

std::string DirectoryListing::getHTMLListing()
{
	std::vector<std::string> listing = getListing(this->_path);
	return (renderListing(listing));
}