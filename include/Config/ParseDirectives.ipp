/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.ipp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:17:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 17:00:00 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEDIRECTIVES_IPP__
# define __PARSEDIRECTIVES_IPP__

# include <ParseContent.hpp>
# include <LocationConfig.hpp>
# include <ServerConfig.hpp>
# include <Utils.hpp>

template <typename T>
void	ParseContent::save_root(std::string head, T &config) {
	StrVector	data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	config.setRootDir(data[1]);
};

template <typename T>
void	ParseContent::save_autoindex(std::string head, T &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	std::string boolean = data[1];
	for (size_t i = 0; i < boolean.length(); i++)
		boolean[i] = tolower(boolean[i]);
	if (!boolean.compare("false"))
		config.setAutoIndex(false);
	else if (!boolean.compare("true"))
		config.setAutoIndex(true);
	else
		throw std::logic_error("Invalid argument \"" + boolean + "\" expected: true|false");
};

template <typename T>
void	ParseContent::save_error_page(std::string head, T &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);

	if (data.size() < 3)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	std::string	value = data[data.size() - 1];
	for (size_t i = 1; i < data.size() - 1; i++)
	{
		int key = std::stoui<int>(data[i]);
		if (key < 400 || key > 599)
			throw logic_error("Value " + data[i] + " not valid for error_page");
		config.addErrorPage(key, config.getRootDir() + value);
	}
};

template <typename T>
void	ParseContent::checkDuplicate(std::string name, T &config) {
	if (config.isSet(name) && ParseContent::canRepeatList.at(name)== false)
		throw std::logic_error("Duplicated directive " + name);
}

#endif