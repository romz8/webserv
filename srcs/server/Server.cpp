/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/13 21:04:41 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const Config& conf) : _serverName(conf.serverName), _servAddr(setServAddr(conf)), _sock(socketFactory(_servAddr))
{
	this->_host = conf.host;
	this->_port = conf.port;
	this->_serverName = conf.serverName;
	this->_hostName = conf.hostName;
	this->_root = conf.root;
	this->_errPageGlobal = conf.errPageGlobal;
	this->_maxBodySize = MAX_BODY_SIZE; //to replace with config max body size
	
	std::vector<std::string> methods1;
	std::vector<std::string> methods2;
	methods1.push_back("GET");
	methods2.push_back("GET");
	methods2.push_back("POST");

	std::map<int, std::string> custom404;
	//custom404.insert(std::pair<int, std::string>(404, "error_pages/409.html"));
	// _locations.push_back(Location("/", methods1, _root, "index.html", true, ""));
	_locations.push_back(Location("/getorder", methods2, _root, "index.html", false, true,"./upload/formData/"));
    _locations.push_back(Location("/postfile", methods2, _root, "index.html", false, true, "./upload/files/"));
	//custom404.insert(std::pair<int, std::string>(404, "error_pages/409.html"));
	//_locations[0].addErrPage(custom404);
    //_locations.Location("/delete", {"DELETE"}, serverRoot, "test.html", false);
	// _locations.push_back(Location("/nimp", methods1, _root, "index.html", true, ""));
    // _locations.push_back(Location("/nimo2", methods2, _root, "index.html", false, "./upload/"));
    // _locations.push_back(Location("/blabla", methods2, _root, "index.html", false, "./upload/"));
	_rootloc = Location("/", methods2, _root, "index.html", true, true, "");
	_rootloc.setCgi(conf.cgiConf[0]);
	_rootloc.setCgi(conf.cgiConf[1]);
	_rootloc.setCgi(conf.cgiConf[2]);
	_rootloc.addErrPage(_errPageGlobal);

	
	printSockAddrIn(_servAddr);
}

Server::~Server(){}

void	Server::_initServ()
{
	_host.clear();
	_port.clear();
	_serverName.clear();
}

const sockaddr_in Server::setServAddr(const Config& conf)
{
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET; //how for ipv6 ?
	servAddr.sin_port = htons(atoi(conf.port.c_str())); // later replace on by server config
	
	//handling IP (either from 0 / nothing or "127.25.24") -> Will need to include getaddress()
	if (conf.host.empty() || conf.host == "0")
		servAddr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(servAddr.sin_family, conf.host.c_str(), &servAddr.sin_addr);
	std::cout << "CONF is host : " << conf.host << " | port : " << conf.port << std::endl;
	return (servAddr);
}

void	Server::run()
{
	addPollFd(_sock.getSocketFd(), POLLIN);
	while(42)
	{
		int ret = poll(_fdSet.data(), _fdSet.size(), _timeout * 1000);
		if (ret < 0)
			throw (std::runtime_error("Poll failed"));
		else if (ret == 0)
			std::cout << "Waiting Connection ..." << std::endl;
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
	}
}

/*
TO CHANGE AS SUBJECT REQUIRES TO MAINTAIN OPEN -> close when recv reutnr 0 ?
*/

void	Server::readClient(int io_fd)
{
	try
	{
		std::string rawhead; 
		rawhead.clear();
		if (!this->_sock.readHeader(io_fd, rawhead))
		{	
			removePollFd(io_fd);
			close(io_fd);
			//usleep(10000); //because to fast to retrun to fd to kernel when using siege
			return;
		}
		Request request(rawhead, _hostName, _maxBodySize); //to replace with config max body size
		request.printHeader();
		if (request.hasBody())
		{
			std::string body;
			body.clear();
			if(!this->_sock.readBody(io_fd, request.getHeader(), rawhead, body))
			{
				removePollFd(io_fd);
				close(io_fd);
				//usleep(10000); //because to fast to retrun to fd to kernel when using siege
				return;
			}
			request.setBody(body);
		}
		const Location* matchLoc = findLocationForRequest(request.getPath());
		if (matchLoc == NULL)
			request.setLocation(_rootloc);
		else
			request.setLocation(*matchLoc);
		request.buildRequest();
		Response resp(request);
		resp.buildResponse();
		std::string response = resp.getResponse();
		//std::cout << BG_BLUE "Response : " RESET << response << std::endl;
		_clientResp[io_fd] = response;
		setPoll(io_fd, POLLOUT);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		removePollFd(io_fd);
		close(io_fd);
	}
}

void Server::sendClient(int io_fd)
{
	std::string response = _clientResp[io_fd];
	send(io_fd, response.c_str(), response.length(), 0);
	setPoll(io_fd, POLLIN);
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
