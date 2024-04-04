/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 17:52:07 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/04 18:14:02 by rjobert          ###   ########.fr       */
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
	std::string		_extension;
	std::map<int, std::string> _statusMsgs;
	std::map<int, std::string> _errPages;
	std::map<std::string, std::string> _mimeTypes;
	std::map<std::string, std::string> _headers;
	std::string		_content_len;
	std::string		_statusLine;
	std::string		_headerResponse;
	std::string		_body;
	std::string		_response;
	static const std::string	_root; //for testing

public:
	Response(Header& head);
	~Response();
	Response(const Response& src);
	Response& operator=(const Response& src);

	void	buildResponse();
	void	setStatusLine(int sCode);
	void	setBody();
	void	parseExtension();
	std::string getResponse() const;
	void	excecuteGetResponse();
	void	handle200();
	void	handle301();
	void	handleError();
	void	addHeader();
	std::string assembHeader();
	void	finalizeHeader();
	std::map<std::string, std::string> initMimeMaps();
	std::map<int, std::string>initErrMaps();
	std::map<int, std::string> initStatusMaps();
	std::string	getMimeType(const std::string& ext) const;
	std::string	getStatusMessage(int statCode) const;
	std::string getErrorPage(int statcode) const;
};

#endif