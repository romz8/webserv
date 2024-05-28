/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/24 17:23:07 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


/******************** Constructor and init part *******************************/

Server::Server(const ServerConfig& conf) : _servAddr(setServAddr(conf)), _socket_fd(-1)
{
	
	_initServ();
	this->_host = conf.getHost();
	std::cout << "host in server: " << this->_host << std::endl;
	this->_port = conf.getPort();
	this->_hostName = 
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
		close(_socket_fd);
	_clientRequest.clear();
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
	_clientRequest.clear();
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
	_clientRequest = src._clientRequest;
	_clientResponse = src._clientResponse;
	return (*this);
}

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

void	Server::_initLocations(const std::vector<LocationConfig>& locationConf)
{
	std::cout << " Loca conf size is : " << locationConf.size() << std::endl;
	for (size_t i = 0; i < locationConf.size(); ++i)
	{
		Location loc(locationConf[i]);
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


/*===================================================================================================*/
/******************** SERVER PART : interfacing socket with Request/Response *************************/
/*===================================================================================================*/


/**
 * @brief Processes the incoming HTTP request.
 * 
 * This function is handling the entire lifecycle of an HTTP request from a client.
 * It performs the following steps:
 * 1. **Request Initialization**: Creates a `Request` object using the raw HTTP header.
 * 2. **Body Handling**: If the request contains a body, reads the body data from the client socket.
 *    - Calls `readBody` to handle different types of body content (e.g., fixed-length, chunked encoding).
 *    - Sets appropriate status codes if there are errors reading the body.
 * 3. **Location Matching**: Finds the best matching location configuration for the request path.
 *    - Uses `findLocationForRequest` to match the request path to a location.
 *    - If no match is found, sets the root location as the default.
 * 4. **Request Building**: Finalizes the request object.
 * 5. **Response Creation**: Creates a `Response` object using the built request.
 *    - Calls `buildResponse` to generate the HTTP response.
 * 6. **Response Storage**: Stores the response and connection header for later use.
 * 
 * @param rawhead The raw HTTP request header received from the client.
 * @param pfd The pollfd structure associated with the client connection.
 */
void	Server::processRequest(const std::string& rawhead, pollfd& pfd)
{
	const int io_fd = pfd.fd;
	Request request(rawhead, _host, _maxBodySize, _serverName, _port); //to replace with config max body size
	if (request.hasBody())
	{
		std::string body;
		body.clear();
		int retBody = readBody(pfd, request.getHeader(), rawhead, body);
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
	if (matchLoc != NULL && !matchLoc->getAlias().empty())
	{
		request.setPath(matchLoc->getAlias());
		request.setStatus(301);
		std::cout <<BG_GREEN "Alias found : " RESET << matchLoc->getAlias() << std::endl;
	}
	else
		request.buildRequest();
	//request.printRequest(); //TO REMOVE AFTER TEST
	Response resp(request);
	resp.buildResponse();
	std::string response = resp.getResponse();
	//std::cout << BG_BLUE "Response : " RESET << response << std::endl;
	_clientRequest[io_fd] = request.getHeaderField("Connection");
	_clientResponse[io_fd] = resp.getResponse();
}

/**
 * @brief Handles read operations for a specified client socket.
 * 
 * Attempts to read data from the specified socket. If the header read is successful,
 * it processes the data depending on whether the request has additional body content.
 * If reading fails at any point, the client is disconnected and the socket is closed.
 * Upon successful reading, it determines the appropriate response based on the request
 * and prepares the socket for a write operation by setting its event to POLLOUT.
 * @param pfd The pollfd structure associated with the client connection.
 * @exception Catches and logs exceptions related to socket operations.
 */
int	Server::readClient(pollfd& pfd)
{
	try
	{
		
		std::string rawhead; 
		rawhead.clear();
		int retHead = readHeader(pfd, rawhead);
		if (retHead == 1)
			processRequest(rawhead, pfd);
		return (retHead);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
}

/**
 * @brief Handles sending responses to a client's socket.
 * 
 * Sends the prepared response to the specified client socket. After sending,
 * it checks the 'Connection' header to decide whether to close the socket or
 * keep it open for further communications. If 'close' is indicated, it disconnects the client;
 * otherwise, it resets the socket to listen for more data (POLLIN).
 * @param pfd The pollfd structure associated with the client connection.
 */
int	Server::sendClient(pollfd &pfd)
{
	int io_fd = pfd.fd;
	if (_clientResponse.find(io_fd) == _clientResponse.end())
		return (-1);
	std::string response = _clientResponse[io_fd];
	size_t byteSent = send(io_fd, response.c_str(), response.length(), 0);
	if (byteSent < 0)
	{
		std::cerr << "send socket Error :" << strerror(errno) << std::endl;
		return (-1);
	}
	else if (byteSent < response.length())
	{
		std::cout << "Partial send, remaining to send : " << response.length() - byteSent << std::endl;
		_clientResponse[io_fd] = response.substr(byteSent);
		return (0);
	}
	if ( _clientRequest[io_fd].find("close") != std::string::npos)
	{
		_clientRequest.erase(io_fd);
		_clientResponse.erase(io_fd);
		return (-1);
	}
	_clientRequest.erase(io_fd);
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


int Server::getSocketInit()const
{
	return (_socket_fd);
}

/*=========================================================================*/
/******************** SOCKET I/O PART **************************************/
/*=========================================================================*/

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
 * 
 * @return The file descriptor of the accepted client socket, or -1 on error.
 */
const int Server::acceptConnection()
{
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	setNonBlocking(io_socket);
	if (io_socket < 0)
		throw std::runtime_error("Error accepting client request");
	return (io_socket);
}
/**
 * @brief Reads the HTTP header from a client socket.
 * 
 * Reads data from the client socket until the header is fully received.
 * BUFFISZE is used as the buffer size for reading data -> it is ~8k bytes as in NGINX
 * 
 * @param io_socket The client socket file descriptor.
 * @param rawRequest A reference to the string where the raw request will be stored.
 * @return 1 on successful read, 0 on client disconnection, -1 on error.
 */
int	Server::readHeader(pollfd& pfd, std::string &rawRequest)
{
	int byteRead = 1;
	char buffer[BUFSIZE];
	byteRead = recv(pfd.fd, buffer, BUFSIZE - 1, 0);
	if (byteRead < 0)
		return (-1);
	if (byteRead == 0)
		return (0);
	buffer[byteRead] = '\0';
	rawRequest.append(buffer, byteRead);
	return (1);
}

/**
 * @brief Reads the HTTP body from a client socket.
 * 
 * Reads the body of the HTTP request based on the content length or transfer encoding.
 * 
 * @param io_socket The client socket file descriptor.
 * @param header A map containing the HTTP headers previously parsed.
 * @param rawhead The raw HTTP header - used to rebuild the body if header read had partial body.
 * @param body A reference to the string where the body will be stored.
 * @return 1 on successful read, 0 on client disconnection, -1 on error, -2 on timeout.
 */
int	Server::readBody(pollfd &pfd, const std::map<std::string, std::string>& header, const std::string& rawhead, std::string& body)
{
	body = rawhead.substr(rawhead.find("\r\n\r\n") + 4);
	std::string sheader= rawhead.substr(0,rawhead.find("\r\n\r\n"));
	std::cout << "header is : " <<  sheader  << std::endl;
	std::cout << "header size is : " << sheader.size() << std::endl;
	//std::cout << "body is : " << body << std::endl;	

	if (header.find("Transfer-Encoding") != header.end() && header.find("Transfer-Encoding")->second == "chunked")
		return (readChunkEncodingBody(pfd, body));
	else if (header.find("Content-Length") != header.end())
	{
		size_t contentLength = std::stol((header.find("Content-Length")->second).c_str());
		std::cout << "content length is : " << contentLength << std::endl;
		return (readFixedLengthBody(pfd, contentLength, body));
	}
	else
		return (-1);
}
/**
 * @brief Reads a fixed-length body from a client socket.
 * 
 * Reads a fixed amount of data from the client socket based on the content length header.
 * 
 * @param clientSocket The client socket file descriptor.
 * @param contentLength The expected length of the content.
 * @param body A reference to the string where the body will be stored.
 * @return 1 on successful read, 0 on client disconnection, -1 on error, -2 on timeout.
 */
int Server::readFixedLengthBody(pollfd &pfd, size_t contentLength, std::string& body) 
{
    
    int bytesRead = 0;
    size_t totalRead = 0;
	
	std::vector<char> buffer(contentLength + 1); 
	time_t startTime = std::time(NULL);
	std::cout << BLUE "body start in FL read is : " RESET << body << std::endl;
    while (body.size() < contentLength) 
	{
		if (difftime(std::time(NULL), startTime) > _readTimeout)
			return (-2);
		bytesRead = recv(pfd.fd, buffer.data(), BUFSIZE, 0);
		std::cout << "Bytes read : " << bytesRead << std::endl;
		if (bytesRead > 0)
            totalRead += bytesRead;
        else if (bytesRead == 0)
            return (0);
		else if (bytesRead == -1 && pfd.revents == POLLIN) //issue with subjject ?
			continue;
        else
		{
            std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
			return (-1);
		}
		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			body.append(buffer.data(), bytesRead);
		}
    }
    buffer[totalRead] = '\0';
    std::string result(buffer.data(), totalRead);
	body.append(result);
    return (1);
}

/**
 * @brief Reads a chunked-encoded body from a client socket.
 * 
 * Reads and decodes chunked transfer encoding from the client socket.
 * reads chunks of data from the client socket until the entire body is received. 
 * The steps involved are:
 * - Continuously reads data from the socket until the end of the chunked body is found.
 *    - Uses `recv` to read data into a buffer.
 *    - Appends the buffer data to the `data` string.
 *    - Checks for the end of the chunked body marker (`0\r\n\r\n`). 
 * @param clientSocket The client socket file descriptor.
 * @param body A reference to the string where the body will be stored.
 * @return 1 on successful read, 0 on client disconnection, -1 on error.
 *
 */
int Server::readChunkEncodingBody(pollfd &pfd, std::string& body) 
{
	std::string data;
	char buffer[BUFSIZE];
	int bytesRead = 0;
	bool endChunk = (body.find("0\r\n\r\n") != std::string::npos);
	
	data.clear();
	while(!endChunk)
	{
		bytesRead = recv(pfd.fd, buffer, BUFSIZE - 1, 0);
		if (bytesRead == -1 && pfd.revents == POLLIN)
			continue;
		else if (bytesRead < 0)
		{
			std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
			return (-1);
		}
		if (bytesRead == 0)
			return (0);
		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			data.append(buffer, bytesRead);
			if (data.find("0\r\n\r\n") != std::string::npos)
				endChunk = true;
		}
	}
	body.append(data);
	std::cout << "Bytes read : " << bytesRead << std::endl;
	return (1);
}

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
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) 
		throw std::runtime_error("Impossible to set non blocking");
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