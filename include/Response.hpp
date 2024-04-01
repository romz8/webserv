/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:52:07 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/01 17:54:12 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <map>

class Response
{
private:
	std::string		_status;
	std::string		_version;
	std::map<std::string, std::string> _headers;
	std::string		_body;

public:
	Response(Header& head);
	~Response();
	Response(const Response& src);
	Response& operator=(const Response& src);

	void	buildResponse();

	
};

#endif