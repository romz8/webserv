/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:31:46 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/30 17:37:26 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <poll.h>
# include "ServerConfig.hpp"
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <unistd.h> // For read
# include <netdb.h> //for inet_ntoa
# include <sys/types.h> //for getaddress() convert Ip to integer
# include <arpa/inet.h> //for getaddress() convert Ip to integer
# include <fcntl.h> //for setNonBlocking
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "colors.h"

# define MAX_Q 420
# define BUFSIZE 8192
# define MAX_HEADER_SIZE 8193
# define TIMEOUTCGI 8

class Request;

class Server
{
private:
	int	_port;
	std::string	_root;
	std::vector<std::string>	_serverName;
	std::string	_hostName;
	std::string	_host; //is it corrrect to have hostName and host ? how is it done on José side
	size_t		_maxBodySize;
	std::vector<Location>	_locations;
	std::map<int, std::string> _errPageGlobal;
	Location _rootloc; //if no url match a location, use root location
	std::map<int , std::string> _clientConnect;
	std::map<int , std::string> _clientResponse;

	std::map<int, std::string> _inputHead;
	std::map<int, std::string> _inputBody;
	std::map<int, bool> _HeaderRead;
	std::map<int, time_t > _start;
	
	int	_socket_fd;
	sockaddr_in _servAddr;
	struct sockaddr_in _client_addr;
	size_t _addr_size;
	static const int _readTimeout = 5;

public:
	Server(const ServerConfig& conf);
	Server(const Server& src);
	Server& operator=(const Server& src);
	~Server();
	
	void 	_initServ();
	void	_initLocations(const std::vector<LocationConfig>& locations);
	// std::string getRequest();
	// std::string getResponse();
	const Location* findLocationForRequest(const std::string& requestPath) const;
	static const sockaddr_in setServAddr(const ServerConfig& conf);
	int		readClient(pollfd& pfd, Request& request);
	int		sendClient(pollfd& pfd);
	void	handleError(const int io_socket, const int error);
	void	processRequest(Request& request, int io_socket);
	int		getSocketInit()const;
	int		handleTimeout(const int io_socket, Request& request);
	
	/***** Socket I/O ********/
	void	_initSock(); // use later on to clear all sockaddr_in, set to 0 before copy or construct
	// int readHeader(pollfd& pfd, std::string& content);
	// int	readBody(pollfd &pfd, const std::map<std::string, std::string>& header, const std::string& rawhead, std::string& body);
	int		acceptConnection();
	// int	readFixedLengthBody(pollfd &pfd, size_t contentLength, std::string& body);	
	// int	readChunkEncodingBody(pollfd &pfd, std::string& body);
	// bool	_readRequest(char* buffer, int byteSize, int fd);
	friend std::ostream& operator<<(std::ostream& os, const Server& serv);
	std::string getHost() const;
	int getPort() const;
	int getMaxBodySize() const;
	std::vector<std::string> getserverName() const;
	void setClientRequest(int fd, const std::string& request);
	void setClientResponse(int fd, const std::string& response);
};

std::ostream& operator<<(std::ostream& os, const Server& serv);
void printSockAddrIn(const sockaddr_in& addr);
void	setNonBlocking(int fd);

#endif

