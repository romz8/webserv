/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 12:44:52 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/29 14:06:16 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <sstream>

class Header
{
private:
	std::string		_method;
	std::string		_path;
	std::string		_version;
	std::map<std::string, std::string> _headers;	//will be a map later on

public:
	Header(const std::string& rawHeader);
	~Header();
	Header(const Header& src);
	Header& operator=(const Header& src);

	void	parseHeader(const std::string& head);
	void	printHeader() const;
};

#endif