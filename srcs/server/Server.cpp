/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/21 21:04:15 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const ServerConfig& conf) : _servAddr(setServAddr(conf)), _sock(socketFactory(_servAddr))
{
	
	_initServ();
	
	this->_host = conf.getHost();
	this->_port = conf.getPort();
	this->_hostName = _host.substr(0, _host.find(':'));
	this->_serverName = conf.getServerName();
	this->_root = conf.getRootDir();
	this->_errPageGlobal = conf.getErrorPages();
	this->_maxBodySize = conf.getClientMaxBodySize();
	_initLocations(conf.getLocationConf());
	printSockAddrIn(_servAddr);
}

Server::~Server(){}

void	Server::_initServ()
{
	_host.clear();
	_hostName.clear();
	_port = 0;
	_serverName.clear();
	_root.clear();
	_maxBodySize = 0;
	_locations.clear();
	_errPageGlobal.clear();
	_host.clear();
	_fdSet.clear();
	_clientRequest.clear();
	_clientResponse.clear();
}

const sockaddr_in Server::setServAddr(const ServerConfig& conf)
{
	std::cout << "host : " << conf.getHost() << std::endl;
	std::cout << "port : " << conf.getPort() << std::endl;
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET; 
	servAddr.sin_port = htons(conf.getPort()); 
	
	//handling IP (either from 0 / nothing or "127.25.24") -> Will need to include getaddress()
	if (conf.getHost().empty() || conf.getHost() == "0.0.0.0")
		servAddr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(servAddr.sin_family, conf.getHostName().c_str(), &servAddr.sin_addr);
	return (servAddr);
}

void	Server::_initLocations(const std::vector<LocationConfig>& locationConf)
{
	std::cout << " Loca conf size is : " << locationConf.size() << std::endl;
	for (size_t i = 0; i < locationConf.size(); ++i)
	{
		Location loc(locationConf[i]);
		std::cout << "Location : " << loc << std::endl;
		this->_locations.push_back(loc);
		if (loc.getPath() == "/")
			_rootloc = loc;
	}
	std::cout << "Locations size : " << _locations.size() << std::endl;
	for (size_t i = 0; i < _locations.size(); ++i) 
	{
        std::cout << "Location " << i << ": " << _locations[i] << std::endl;
    }
}
/**
 * @brief Executes the main server loop, monitoring and responding to socket events.
 * 
 * continuously polls the set of file descriptors (_fdSet) to detect and handle
 * events. It manages the server's listening socket for new connections and other sockets for
 * read/write operations. 
 * The function uses a blocking poll call with a timeout.
 * In case of errors in poll, it throws a runtime exception, and on timeout, 
 * it logs a waiting message. 
 * Actual socket handling is delegated to other functions based 
 * on the type of event (POLLIN or POLLOUT).
 * 
 * @throws std::runtime_error if poll fails.
 */
void	Server::run()
{
	addPollFd(_sock.getSocketFd(), POLLIN);
	while(42)
	{
		int ret = poll(_fdSet.data(), _fdSet.size(), _timeout * 1000);
		if (ret < 0)
			throw (std::runtime_error("Poll failed"));
		else if (ret == 0)
			std::cout << GREEN "Waiting Connection ..." RESET << std::endl;
		else
		{
			for (size_t i = 0; i < _fdSet.size(); ++i)
			{
				if (_fdSet[i].revents & POLLIN)
				{
					if (_fdSet[i].fd == _sock.getSocketFd())
						this->handleConnection();
					else
						readClient(_fdSet[i].fd);
				}
				else if (_fdSet[i].revents & POLLOUT)
					sendClient(_fdSet[i].fd);
			}
		}
		usleep(1000);
	}
}

void	Server::processRequest(const std::string& rawhead, const int io_fd)
{
	Request request(rawhead, _host, _maxBodySize); //to replace with config max body size
	if (request.hasBody())
	{
		std::string body;
		body.clear();
		int retBody = this->_sock.readBody(io_fd, request.getHeader(), rawhead, body);
		if(retBody == -2)
			request.setStatus(408);
		else if (retBody == -1)
			request.setStatus(400);
		else
			request.setBody(body);
	}
	const Location* matchLoc = findLocationForRequest(request.getPath()); //TO VERIFY AND EDIT WITH CONFIG NEW LOCATION
	if (matchLoc == NULL)
		request.setLocation(_rootloc); // to replace with root location in config logic
	else
	{
		request.setLocation(*matchLoc);
		std::cout << BG_GREEN "Location found : " << matchLoc->getPath() << RESET << std::endl; //TO REMOVE AFTER TEST
	}
	request.buildRequest();
	request.printRequest(); //TO REMOVE AFTER TEST
	Response resp(request);
	resp.buildResponse();
	std::string response = resp.getResponse();
	//std::cout << BG_BLUE "Response : " RESET << response << std::endl;
	_clientRequest[io_fd] = request.getHeaderField("Connection");
	_clientResponse[io_fd] = resp.getResponse();
	setPoll(io_fd, POLLOUT);
}

/**
 * @brief Handles read operations for a specified client socket.
 * 
 * Attempts to read data from the specified socket. If the header read is successful,
 * it processes the data depending on whether the request has additional body content.
 * If reading fails at any point, the client is disconnected and the socket is closed.
 * Upon successful reading, it determines the appropriate response based on the request
 * and prepares the socket for a write operation by setting its event to POLLOUT.
 * 
 * @param io_fd The file descriptor of the client socket.
 * @exception Catches and logs exceptions related to socket operations.
 */
void	Server::readClient(int io_fd)
{
	try
	{
		std::string rawhead; 
		rawhead.clear();
		int retHead = this->_sock.readHeader(io_fd, rawhead);
		if (retHead == 1)
			processRequest(rawhead, io_fd);
		else
			closeClient(io_fd);
			//handleError(io_fd, retHead);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		removePollFd(io_fd);
		close(io_fd);
	}
}

/**
 * @brief Handles sending responses to a client's socket.
 * 
 * Sends the prepared response to the specified client socket. After sending,
 * it checks the 'Connection' header to decide whether to close the socket or
 * keep it open for further communications. If 'close' is indicated, it disconnects the client;
 * otherwise, it resets the socket to listen for more data (POLLIN).
 * 
 * @param io_fd The file descriptor of the client socket to send data to.
 */
void Server::sendClient(int io_fd)
{
	if (_clientResponse.find(io_fd) == _clientResponse.end())
	{
		closeClient(io_fd);
		return;
	}
	std::string response = _clientResponse[io_fd];
	//std::cout << YELLOW "Response : " << response << RESET << std::endl;
	size_t byteSent = send(io_fd, response.c_str(), response.length(), 0);
	if (byteSent < 0)
	{
		std::cerr << "send socket Error :" << strerror(errno) << std::endl;
		closeClient(io_fd);
		return;
	}
	else if (byteSent < response.length())
	{
		std::cout << "Partial send, remaining to send : " << response.length() - byteSent << std::endl;
		_clientResponse[io_fd] = response.substr(byteSent);
		setPoll(io_fd, POLLOUT);
		return;
	}
	if ( _clientRequest[io_fd].find("close") != std::string::npos)
		closeClient(io_fd);
	else
		setPoll(io_fd, POLLIN);
	//std::cout << " bytesent : " << byteSent << std::endl;
	//std::cout << " response sent : " << response << std::endl;
	_clientRequest.erase(io_fd);
	_clientResponse.erase(io_fd);
}

Socket Server::socketFactory(const sockaddr_in& addr) 
{
    try 
	{
        return Socket(addr);
    } 
	catch(const std::exception& e) 
	{
        std::cerr << this->_serverName;
		std::cerr << "Failed to create socket: " << e.what() << std::endl;
        throw;
    }
}

/*
Pointer to store the best matchLength of the longest path match found
Check if the location path is a prefix of the request path with compare()
Update the length of the longest match
Return the best match found, or NULL if no match was found
*/
const Location* Server::findLocationForRequest(const std::string& requestPath) const 
{
    const Location* bestMatch = NULL;  // Pointer to store the best match
    size_t longestMatchLength = 0;     // Length of the longest path match found

    for (size_t i = 0; i < _locations.size(); ++i) 
	{
        const std::string& locationPath = _locations[i].getPath();
       
        if (requestPath.compare(0, locationPath.length(), locationPath) == 0) 
		{
			if (locationPath.length() > longestMatchLength) 
			{
                bestMatch = &_locations[i];
                longestMatchLength = locationPath.length();  
            }
        }
    }
	return bestMatch; 
}

/**
 * @brief Adds a new file descriptor and its interested events to the polling set.
 * 
 * Registers a new file descriptor with specified events (POLLIN, POLLOUT) into the _fdSet,
 * enabling the main loop to monitor this new descriptor for the given events.
 * 
 * @param fd The file descriptor to add.
 * @param events The polling events to watch for (POLLIN, POLLOUT).
 */
void	Server::addPollFd(int fd, short events)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	_fdSet.push_back(pfd);
}

void Server::removePollFd(int fd)
{
	for (size_t i = 0; i < _fdSet.size(); ++i)
	{
		if (_fdSet[i].fd == fd)
		{
			_fdSet.erase(_fdSet.begin() + i);
			break;
		}
	}
}
/**
 * @brief Accepts a new connection and adds it to the polling set.
 * 
 * This function is called when there is an incoming connection request on the server's listening socket.
 * It accepts the connection, sets the new socket to non-blocking mode, and adds it to the set of polled
 * file descriptors, listening for incoming data (POLLIN).
 */
void	Server::handleConnection()
{
	try 
	{
		int io_fd = this->_sock.acceptConnection();
		setNonBlocking(io_fd);
		this->addPollFd(io_fd, POLLIN);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

/**
 * @brief Removes a file descriptor from the polling set.
 * 
 * Searches for and removes the specified file descriptor from the _fdSet, effectively
 * stopping any further monitoring of this descriptor in the server loop.
 * 
 * @param fd The file descriptor to remove.
 */
void	Server::setPoll(int fd, short events)
{
	for (size_t i = 0; i < _fdSet.size(); ++i)
	{
		if (_fdSet[i].fd == fd)
		{
			_fdSet[i].events = events;
			break;
		}
	}
}

void Server::closeClient(int io_fd)
{
	removePollFd(io_fd);
	close(io_fd);
	//usleep(1000); //because to fast to retrun to fd to kernel when using siege
}

std::ostream& operator<<(std::ostream& os, const Server& serv)
{
	os << "Server : " << serv._serverName << std::endl;
	os << "Host : " << serv._host << std::endl;
	os << "Port : " << serv._port << std::endl;
	os << "Root : " << serv._root << std::endl;
	os << "Max Body Size : " << serv._maxBodySize << std::endl;
	os << "Error Pages : " << std::endl;
	for (std::map<int, std::string>::const_iterator it = serv._errPageGlobal.begin(); it != serv._errPageGlobal.end(); ++it)
		os << it->first << " : " << it->second << std::endl;
	os << "Locations : " << std::endl;
	for (size_t i = 0; i < serv._locations.size(); ++i)
		os << serv._locations[i] << std::endl;
	return os;
}
// void Server::handleError(const int io_socket))
// {
// 	std::string response;
	
// 	if (error == 0)
// 	{
// 		closeClient(io_socket);
// 		return;
// 	}
// 	if (error == -2)
// 		response = "HTTP/1.1 408 Request Timeout\r\nContent-Length: 0\r\n\r\n";
// 	else 
// 		response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
// 	_clientRequest[io_socket] = "close";
// 	_clientResponse[io_socket] = response;
// 	setPoll(io_socket, POLLOUT);
// }

