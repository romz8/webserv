#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
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
	std::map<std::string, std::string> _mimeTypes;
	std::map<std::string, std::string> _headers;
	std::string		_content_len;
	std::string		_statusLine;
	std::string		_headerResponse;
	std::string		_body;
	std::string		_response;
	Location		_location;

public:
	Response(Request& head);
	~Response();
	Response(const Response& src);
	Response& operator=(const Response& src);   

	void	buildResponse();
	void	setStatusLine(int sCode);
	void	setBody();
	std::string	readWebFile(const std::string& path);
	std::string getResponse() const;
	void	excecuteResponse();
	void	handleError();
	void	addHeaders();
	std::string assembHeaders();
	void	finalizeResponse();
	std::map<std::string, std::string> initMimeMaps();
	std::map<int, std::string>initErrMaps();
	std::map<int, std::string> initStatusMaps();
	std::string	getMimeType(const std::string& ext) const;
	std::string	getStatusMessage(int statCode) const;
	std::string getErrorPage(int statcode) const;
};

#endif