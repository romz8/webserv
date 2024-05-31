/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/31 10:37:50 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


/******************** Constructor and init part *******************************/

Server::Server(const ServerConfig& conf) :  _socket_fd(-1), _servAddr(setServAddr(conf))
{
	
	_initServ();
	this->_host = conf.getHost();
	std::cout << "host in server: " << this->_host << std::endl;
	this->_port = conf.getPort(); 
	this->_hostName = _host.substr(0, _host.find(':'));
	this->_serverName = conf.getServerName();
	this->_root = conf.getRootDir();
	this->_errPageGlobal = conf.getErrorPages();
	this->_maxBodySize = conf.getMaxBodySize();
	_initLocations(conf.getLocationConf());
	
	printSockAddrIn(_servAddr);
}

Server::~Server()
{
	if (_socket_fd != -1)
	{
		std::cout << RED "Closing socket : " RESET << _socket_fd << std::endl;
		close(_socket_fd);
	}
	_clientConnect.clear();
	_clientResponse.clear();
}

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
	_clientConnect.clear();
	_clientResponse.clear();
	
}

Server::Server(const Server& src)
{
	*this = src;
}

Server& Server::operator=(const Server& src)
{
	if (this == &src)
		return (*this);
	_host = src._host;
	_port = src._port;
	_root = src._root;
	_serverName = src._serverName;
	_hostName = src._hostName;
	_maxBodySize = src._maxBodySize;
	_locations = src._locations;
	_servAddr = src._servAddr;
	_socket_fd = src._socket_fd;
	_client_addr = src._client_addr;
	_addr_size = src._addr_size;
	_errPageGlobal = src._errPageGlobal;
	_rootloc = src._rootloc;
	_clientConnect = src._clientConnect;
	_clientResponse = src._clientResponse;
	return (*this);
}



void	Server::_initLocations(const std::vector<LocationConfig>& locationConf)
{
	for (size_t i = 0; i < locationConf.size(); ++i)
	{
		Location loc(locationConf[i]);
		this->_locations.push_back(loc);
		if (loc.getPath() == "/")
			_rootloc = loc;
	}
	for (size_t i = 0; i < _locations.size(); ++i)
	{
        std::cout << "Location " << i << ": " << _locations[i] << std::endl;
    }
}


/*===================================================================================================*/
/******************** SERVER PART : interfacing socket with Request/Response *************************/
/*===================================================================================================*/



/**
 * @brief Handles read operations for a specified client socket. 
 * 1. **Initialization**:
 *    - Checks if the start time for the given fd exists in the `_start` map
 *      or if the request status is `0` (indicating a new request).
 *    - If either condition is true, initializes the start time for the file descriptor to the current time.
 * 
 * 2. **Reading Data**:
 *    - Attempts to read data from the client socket using `recv`.
 *    - If `recv` returns a negative value, logs the error, removes the start time entry, and returns `-1`.
 *    - If `recv` returns `0`, indicating the client has disconnected, logs the disconnection, 
 * 	removes the start time entry, and returns `0`.
 *    - otherwise read data is stored in a buffer and terminated with a null character for nbr of bytes read.
 * 
 * 3. **Processing the Request**:
 *    - Calls `request._readRequest` to process the read data.
 *    - If `_readRequest` returns `true` (indicating the request is fully read and valid):
 *      - Removes the start time entry.
 *      - Calls `processRequest` to handle the request.
 *      - If the request requires CGI execution (`request.execCgi()` returns `true`), logs the CGI launch and returns `2` to indicate the CGI process.
 *      - Otherwise, returns `1` to indicate a successfully processed request.
 *    - If the request is not fully read, returns `2` to continue reading.
 * 
 * 4. **Exception Handling**:
 *    - Catches any exceptions, logs the error message, and returns `-1`.
 */
int	Server::readClient(pollfd& pfd, Request& request)
{
	try
	{
		if (_start.find(pfd.fd) == _start.end() || request.getStatus() == 0)
			_start[pfd.fd] = std::time(NULL);
		int byteRead;
		char buffer[BUFSIZE];
		byteRead = recv(pfd.fd, buffer, BUFSIZE - 1, 0);
		if (byteRead < 0 )
		{
			std::cerr << "recv socket Error : in fd " << pfd.fd << std::endl;
			_start.erase(pfd.fd);
			return (-1);
		}
		if (byteRead == 0)
		{
			std::cout << "Client disconnected for fd"<< pfd.fd << std::endl;
			_start.erase(pfd.fd);
			return (0);
		}
		buffer[byteRead] = '\0';
		//std::cout << BLUE << "bytes read : " << byteRead << RESET << std::endl;
		if (request._readRequest(buffer, byteRead))
		{
			_start.erase(pfd.fd);
			processRequest(request, pfd.fd);
			if (request.execCgi())
				return (2);
			return (1);
		}
		return (2);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
}


/**
 * @brief Handles sending the HTTP responses to a client's socket.
 * 
 * 1. **Response Preparation**:
 *    - Checks if a response exists for the given fd in the `_clientResponse` map.
 *    - If no response is found, returns `-1` to indicate an error.
 *    - Retrieves the response string from the map.
 * 
 * 2. **Sending Data**:
 *    - Attempts to send the response to the client socket using `send`.
 *    - If `send` returns a negative value, logs the error and returns `-1`.
 *    - If `send` returns a value less than the response length, indicating a partial send, 
 * 		updates the response in the `_clientResponse` map with the unsent portion. 
 * 		Returns `0` to continue sending.
 * 
 * 3. **Connection Management**:
 *    - If the `Connection` header indicates "close",removes the connection
 *  and response entries from their respective maps, and returns `-1` to close the connection.
 *    - Otherwise, removes the connection and response entries from their respective maps 
 * and returns `1` to indicate the response was sent successfully and the connection should remain open.
 */
int	Server::sendClient(pollfd &pfd)
{
	int io_fd = pfd.fd;
	if (_clientResponse.find(io_fd) == _clientResponse.end())
		return (-1);
	std::string response = _clientResponse[io_fd];
	int byteSent = send(io_fd, response.c_str(), response.length(), 0);
	if (byteSent < 0)
	{
		std::cerr << "send socket Error : in fd " << io_fd << std::endl;
		return (-1);
	}
	else if ((size_t)byteSent < response.length())
	{
		std::cout << "Partial send, remaining to send : " << response.length() - byteSent << std::endl;
		_clientResponse[io_fd] = response.substr(byteSent);
		return (0);
	}
	if ( _clientConnect[io_fd].find("close") != std::string::npos)
	{
		std::cout << "Closing connection for fd : " << io_fd << std::endl;
		_clientConnect.erase(io_fd);
		_clientResponse.erase(io_fd);
		return (-1);
	}
	_clientConnect.erase(io_fd);
	_clientResponse.erase(io_fd);
	return (1);
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
 * @brief Processes the incoming HTTP request.
 * 
 * This function is handling the entire lifecycle of an HTTP request from a client.
 * It performs the following steps:
 * 1. **Request Initialization**: Creates a `Request` object using the raw HTTP header.
 * 2. **Body Handling**: If the request contains a body, it sets the body content in the request object.
 * 3. **Location Matching**: Finds the best matching location configuration for the request path.
 *    - Uses `findLocationForRequest` to match the request path to a location.
 *    - If no match is found, sets the root location as the default.
 *   - If an alias is found, updates the request path and status code accordingly.
 * 4. **Request Building**: Finalizes the request object through HTTP Methods check
 * 	  - if the request is executing a CGI, it returns to the caller (the response is handled in Cluster to
 * be non-blocking
 * 5. **Response Creation**: Creates a `Response` object using the built request.
 *    - Calls `buildResponse` to generate the HTTP response.
 * 6. **Response Storage**: Stores the response and connection header for later use.
 * 
 * @param rawhead The raw HTTP request header received from the client.
 * @param pfd The pollfd structure associated with the client connection.
 */
void	Server::processRequest(Request& request, int io_fd)
{
	if (request.hasBody())
		request.setBody(request.getrawBody());
	const Location* matchLoc = findLocationForRequest(request.getPath());
	if (matchLoc == NULL)
		request.setLocation(_rootloc); // to replace with root location in config logic
	else
		request.setLocation(*matchLoc);
	if (matchLoc != NULL && !matchLoc->getAlias().empty())
	{
		request.setPath(matchLoc->getAlias());
		request.setStatus(301);
	}
	else
		request.buildRequest();
	if (request.execCgi())
		return;
	std::cout << request << std::endl;
	Response resp(request);
	resp.buildResponse();
	std::string response = resp.getResponse();
	_clientConnect[io_fd] = request.getHeaderField("Connection");
	_clientResponse[io_fd] = resp.getResponse();
	request.initRequest();
}


/**
 * @brief Handles read timeouts for client connections.
 * 
 * This function checks if a read timeout has occurred for a given client connection
 * (identified by `io_fd`). If a timeout is detected, it sets the appropriate status
 * code, prepares a response, and updates the connection state. The function performs
 * the following logical steps:
 * 
 * 1. Checks if the start time for the given file descriptor (`io_fd`) exists in the `_start` map.
 *    - If the start time is not found, it returns `0`, indicating no timeout handling is needed.
 * 2. Calculates the elapsed time since the start of the read operation using `std::time(NULL)`.
 * 3. Compares the elapsed time to the configured read timeout (`_readTimeout`).
 *    - If the elapsed time exceeds the read timeout, it proceeds to handle the timeout:
 *      - Sets the request status to `408` (Request Timeout).
 *      - Creates a `Response` object using the timed-out request and builds the HTTP response.
 *      - Updates the `_clientConnect` map to indicate the connection should be closed.
 *      - Stores the prepared response in the `_clientResponse` map.
 *      - Resets the request by calling `request.initRequest()` to prepare it for future use.
 *      - Removes the start time entry from the `_start` map to clean up the state.
 *      - Returns `-1` to indicate that the timeout was handled and the connection should be closed.
 * 4. If no timeout has occurred, it returns `0`, indicating that no action is needed.
 */
int	Server::handleTimeout(const int io_fd, Request& request)
{
	if (_start.find(io_fd) == _start.end())
		return(0);
	if (std::time(NULL) -  _start[io_fd] > _readTimeout)
	{
		request.setStatus(408);
		Response resp(request);
		resp.buildResponse();
		_clientConnect[io_fd] = "close";
		_clientResponse[io_fd] = resp.getResponse();
		request.initRequest();
		_start.erase(io_fd);
		return (-1);
		
	}
	return(0);
}

/*=========================================================================*/
/******************** SOCKET I/O PART **************************************/
/*=========================================================================*/

/**
 * @brief Initializes server address structure from server configuration.
 * 
 * Sets up the sockaddr_in structure using the provided ServerConfig, converting
 * the hostname and port into appropriate network representations.
 */
const sockaddr_in Server::setServAddr(const ServerConfig& conf)
{
	sockaddr_in servAddr;
	struct addrinfo hints, *res;
	memset(&servAddr, 0, sizeof(servAddr));
	memset(&hints, 0, sizeof(hints));
	servAddr.sin_family = AF_INET; 
	servAddr.sin_port = htons(conf.getPort());

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	std::string port = std::to_string(conf.getPort());
	std::string hostname = conf.getHostName();
	
	if (hostname.empty())
		hostname = "localhost";
	int gaierr = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res);
	if (gaierr != 0)
	{
		freeaddrinfo(res);
		std::string msg = gai_strerror(gaierr);
		throw std::runtime_error("Error getaddrinfo() : " + msg);
	}
	servAddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	freeaddrinfo(res);
	return (servAddr);
}

void	Server::_initSock()
{
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd < 0)
		throw std::runtime_error("Error creating socket");
	this->_addr_size = sizeof(struct sockaddr_in);
	
	printSockAddrIn(_servAddr);
	setNonBlocking(this->_socket_fd);
	int enable = 1;
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::string catchsys = strerror(errno);
		throw std::runtime_error("Error setsockopt() on the socket" + catchsys);
	}
	if (bind(this->_socket_fd, (const struct sockaddr *) &_servAddr, _addr_size) < 0)
	{
		std::string catchsys = strerror(errno);
		throw std::runtime_error("Error binding the socket : " + catchsys);
		std::cout << "Error binding the socket" << std::endl;
	}
	
	if (listen(this->_socket_fd, MAX_Q) < 0)
		throw std::runtime_error("Error listening socket stage");
}

/**
 * @brief Accepts a new client connection.
 * 
 * Accepts a connection on the server socket, sets the new socket to non-blocking mode,
 * and returns the client socket file descriptor.
 * we then create the request object that will handle the request and store it in a map
 * this allow to "stream" the request building as the data is read from the socket
 * @return The file descriptor of the accepted client socket, or -1 on error.
 */
int Server::acceptConnection()
{
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	setNonBlocking(io_socket);
	if (io_socket < 0)
		throw std::runtime_error("Error accepting client request");
	//Request request(_host, _maxBodySize, _serverName, _port);
	//_inputRequest[io_socket] = request;
	std::cout << BLUE  "New connection accepted on fd : " RESET << io_socket << std::endl;
	return (io_socket);
}


/********************** GETTERS & SETTERS ****************************/

std::string Server::getHost() const
{
	return (_host);
}

int	Server::getPort() const
{
	return (_port);
}

int Server::getMaxBodySize() const
{
	return (_maxBodySize);
}

std::vector<std::string> Server::getserverName() const
{
	return (_serverName);
}

void Server::setClientRequest(int fd, const std::string& request)
{
	this->_clientConnect[fd] = request;
}

void Server::setClientResponse(int fd, const std::string& response)
{
	this->_clientResponse[fd] = response;
}

int Server::getSocketInit()const
{
	return (_socket_fd);
}


/******************* UTILS ***************************************/

void printSockAddrIn(const sockaddr_in& addr) 
{
    // Convert the IP address to a string
    char* ipStr = inet_ntoa(addr.sin_addr);

    // Convert the port number from network byte order to host byte order
    unsigned int port = ntohs(addr.sin_port);

    std::cout << "IP Address: " << ipStr << ", Port: " << port << std::endl;
}

void	setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Impossible to get flags");
	if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) 
		throw std::runtime_error("Impossible to set non blocking");
}
std::ostream& operator<<(std::ostream& os, const Server& serv)
{
	os << "Server : " << serv._serverName[0] << std::endl;
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

