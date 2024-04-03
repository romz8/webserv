/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:52:07 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/03 20:20:05 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Header.hpp"
# include <fstream>



class Response
{
private:
	int				_status;
	std::string		_method;
	std::string		_statusMsg;
	std::string		_version;
	std::string		_assetPath;
	int				_content_length;
	std::string		_content_type;
	std::map<std::string, std::string> _headers;
	std::string		_statusLine;
	std::string		_headerContent;
	std::string		_headerResponse;
	std::string		_body;
	std::string		_reponse;

public:
	Response(Header& head);
	~Response();
	Response(const Response& src);
	Response& operator=(const Response& src);

	void	buildResponse();
	void	setStatusLine(int sCode);
	void	setBody();
	std::string getResponse() const;
	void	excecuteGetResponse();
	void	handle200();
	void	handle301();
	void	handleError();
	

	
};

#endif