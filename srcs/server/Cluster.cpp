/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:20:31 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/31 12:54:25 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include <Signal.hpp>

Cluster::Cluster(std::vector<ServerConfig> serverConfig) 
{
	_servers.clear();
	_fdSet.clear();
	_fdtoServ.clear();
	_fdtoReq.clear();
	for (std::vector<ServerConfig>::iterator it = serverConfig.begin(); it != serverConfig.end(); ++it)
		addServer(*it);
}

Cluster::Cluster(const Cluster& src)
{
	*this = src;
}

Cluster& Cluster::operator=(const Cluster& src)
{
	if (this == &src)
		return (*this);
	_servers = src._servers;
	return (*this);
}

Cluster::~Cluster()
{
	std::cout << BG_RED "destroying cluster" << std::endl;
	for (std::vector<pollfd>::size_type i = 0; i < _fdSet.size(); i++)
	{
		std::cout << "closing fd " RESET << _fdSet[i].fd << std::endl;	
		close(_fdSet[i].fd);
	}
	_servers.clear();
	_fdSet.clear();
	_fdtoServ.clear();
	_fdtoReq.clear();
}

std::vector<Server> Cluster::getServers() const
{
	return (_servers);
}

void Cluster::addServer(const ServerConfig& server)
{
	_servers.push_back(Server(server));
}

void	Cluster::setUpServer() // should we throw or handle any issue here ? how ?
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->_initSock();
		addPollFd(it->getSocketInit(), POLLIN, &(*it), INIT); 
	}
}

/**
 * @brief Executes the main event loop for the Cluster, managing multiple server instances.
 * 
 * The `run()` function is responsible for continuously monitoring and handling incoming connections
 * and I/O events for multiple server instances using the `poll()` system call. This function sets up
 * the server sockets, adds them to the poll set, and enters an infinite loop where it handles
 * various events such as new connections, data reads, and data writes.
 * 
 * The function performs the following steps:
 * 1. **Server Setup**: Calls `setUpServer()` to initialize server sockets and add them to the poll set.
 * 2. **Polling Loop**: Enters an infinite loop to continuously monitor file descriptors.
 * 3. **Poll Call**: Uses `poll()` to wait for events on the file descriptors with a specified timeout.
 *    - On error (`ret < 0`), it clears the poll set and reinitializes it with the server sockets.
 *    - On timeout (`ret == 0`), it logs a message indicating that it is waiting for connections.
 * 4. **Event Handling**:
 *    - Iterates through the poll set to check for triggered events.
 *    - For each file descriptor, it finds the corresponding server from `_fdtoServ`.
 *    - **POLLIN Event**: 
 *      - If the event is on a server socket, it accepts new connections and adds them to the poll set.
 *      - If the event is on a client socket, it reads data from the client.
 *    - **POLLOUT Event**: Sends data to the client.
 *    - **Error Events**: Handles errors such as `POLLERR`, `POLLHUP`, and `POLLNVAL` by removing the client.
 * 5. **Revents Reset**: Resets the `revents` field of each pollfd structure after processing.
 * 6. **Sleep**: Adds a small sleep interval (`usleep(1000)`) to prevent busy-waiting.
 * 7. **Timeout Handling**: Checks for timeouts on client connections and sends responses if needed.
 */
void Cluster::run()
{
	setUpServer();
	Signal::reciveSigs();
	
	while (Signal::Running)
	{
		int ret = poll(_fdSet.data(), _fdSet.size(), _timeout * 1000);
		if (ret < 0)
		{
			_fdSet.clear();
			for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
				addPollFd(it->getSocketInit(), POLLIN, &(*it), INIT);
		}
		checkCGIState();
		if (ret == 0)
			std::cout << GREEN "Waiting Connection ..." RESET << std::endl;
		else
		{
			for (std::vector<pollfd>::size_type i = 0; i < _fdSet.size(); i++)
			{
				std::map<int, Server*>::iterator it = _fdtoServ.find(_fdSet[i].fd);
				if (it == _fdtoServ.end())
				{
					std::cerr << "Error: no mapped Server to connection" << std::endl;
					removePollFd(_fdSet[i].fd);
					continue;
				}
				Server *serv = it->second;
				if (_fdSet[i].revents & POLLIN)
				{
					if (_fdSet[i].fd == serv->getSocketInit())
						{
							int newFd = serv->acceptConnection();
							if (newFd < 0)
							{
								std::cerr << "Accept error "<< strerror(errno) << std::endl;
								removeClient(_fdSet[i].fd);
							}
							else
								addPollFd(newFd, POLLIN, serv, READY);
						}
					else
					{
						int ret = serv->readClient(_fdSet[i], _fdtoReq[_fdSet[i].fd]);
						if (ret <= 0)
							removeClient(_fdSet[i].fd);
						else if (ret == 1)
							setPoll(_fdSet[i].fd, POLLOUT);
						else
							continue;
					}
				}
				else if (_fdSet[i].revents & POLLOUT)
				{
					int ret = serv->sendClient(_fdSet[i]);
					if (ret < 0)
						removeClient(_fdSet[i].fd);
					else if (ret == 1)
						setPoll(_fdSet[i].fd, POLLIN);
				}
				else if (_fdSet[i].revents & (POLLERR | POLLHUP | POLLNVAL))
					removeClient(_fdSet[i].fd);
			}
		}
		for (std::vector<pollfd>::size_type i = 0; i < _fdSet.size(); i++)
		{
			std::map<int, Server*>::iterator it = _fdtoServ.find(_fdSet[i].fd);
			if (it != _fdtoServ.end())
			{
				Server *serv = it->second;
				if (serv->handleTimeout(_fdSet[i].fd, _fdtoReq[_fdSet[i].fd]) < 0)
				{
					int ret = serv->sendClient(_fdSet[i]);
					if (ret < 0)
						removeClient(_fdSet[i].fd);
					else if (ret == 1)
						setPoll(_fdSet[i].fd, POLLIN);
				}
				_fdSet[i].revents = 0;
			}
		}
		usleep(1000);
	}
}

/**
 * @brief Adds a file descriptor and associated events to the polling set. it maps 
 * the file descriptor to a `Server` instance to handle the events. in READY state, 
 * it also maps the file descriptor to a `Request` instance to handle the request.
 * the request will then  be handled by the request instance based on server rule
 * 
 * The `addPollFd` function registers a new file descriptor along with the events it should be 
 * monitored for (e.g., `POLLIN`, `POLLOUT`) into the `_fdSet`, which is used by the `poll()` system call.
 * Then it maps the file descriptor to a `Server` instance to handle the events.
 */
void	Cluster::addPollFd(int fd, short events, Server* server, servState state)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	_fdSet.push_back(pfd);
	_fdtoServ[fd] = server;
	if (state == READY)
		_fdtoReq.insert(std::make_pair(fd, Request(server->getHost(), server->getMaxBodySize(), server->getserverName(), server->getPort())));
	std::cout << BG_RED "created new pollfd for fd " RESET << fd << std::endl;
	// std::cout << "server conf is : " << server->getHost() << " " << server->getPort() << std::endl;
	// std::cout << "fdtoServ size is : " << _fdtoServ.size() << std::endl;
	//std::cout << "request set up is : " << _fdtoReq[fd] << std::endl;
}

/**
 * @brief Removes a file descriptor from the polling set.
 * 
 * The `removePollFd` function searches for and removes the specified file descriptor from the `_fdSet`, 
 * effectively stopping the monitoring of this descriptor in the polling loop.
 * 
 * @param fd The file descriptor to remove from the polling set.
 */
void Cluster::removePollFd(int fd)
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
 * @brief Updates the events associated with a file descriptor in the polling set.
 * 
 * The `setPoll` function updates the events to be monitored for a specified file descriptor. 
 * It modifies the `events` field of the corresponding `pollfd` structure within the `_fdSet`.
 */
void	Cluster::setPoll(int fd, short events)
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
/**
 * @brief Removes a client file descriptor from the polling set and closes it.
 * 
 * The `removeClient` function handles the cleanup for a client connection. It removes the 
 * client's file descriptor from the `_fdSet`, closes the socket, and erases the mapping 
 * from `_fdtoServ`. This function ensures that all resources associated with the client 
 * are properly released.
 * 
 * @param fd The client file descriptor to remove and close.
 */
void	Cluster::removeClient(int fd)
{
	
	close(fd);
	removePollFd(fd);
	_fdtoServ.erase(fd);
	_fdtoReq.erase(fd);
}

/**
 * @brief Checks the state of ongoing CGI processes and handles their completion.
 * this is becasuse if it was done in the readClient function, it would block the
 * server from handling other requests. It has to go through the main loop with poll
 * 
 * This function iterates through all requests tracked by the `_fdtoReq` map to
 * monitor the state of CGI processes. It performs the following logical steps:
 * 
 * 1. Iterates through the map of file descriptors to requests.
 * 2. Checks if a request is currently executing a CGI process (`_onCGI` is true)
 *    and if the `execCgi()` function returns true, indicating that the CGI process is active.
 * 3. If the CGI process has been running longer than `TIMEOUTCGI`, it:
 *    - Marks the CGI process as done (`cgiDone = true`).
 *    - Kills the CGI process using `kill` with `SIGKILL`.
 *    - Sets the request status to 504 (Gateway Timeout).
 * 4. If the CGI process has completed (`waitpid` returns > 0), it:
 *    - Marks the CGI process as done (`cgiDone = true`).
 *    - Reads the CGI process output from the file descriptor `_fdout[0]`.
 *    - Checks if the CGI process exited with a non-zero status or if reading failed.
 *      If so, sets the request status to 502 (Bad Gateway).
 *    - Otherwise, reads the output into a buffer, stores it in the request's `_respbody`,
 *      and sets the request status to 200 (OK).
 * 5. If the CGI process is done (`cgiDone` is true):
 *    - Closes the CGI output file descriptor `_fdout[0]`.
 *    - Calls `checkCGISHeader` on the request to validate the CGI response headers.
 *    - Constructs a `Response` object using the request and builds the HTTP response.
 *    - If the status is 502 or 504, sets the connection to close after sending the response.
 *      Otherwise, sets the connection header based on the request's `Connection` field.
 *    - Stores the prepared response in `_clientResponse`.
 *    - Initializes the request for future use.
 *    - Changes the poll event to `POLLOUT` to indicate readiness to send the response.
 */
void Cluster::checkCGIState()
{
	for (std::map<int, Request>::iterator it = _fdtoReq.begin(); it != _fdtoReq.end(); ++it)
	{
		if (!(it->second.getCgi()._onCGI && it->second.execCgi()))
			continue;
		bool cgiDone = false;
		int status;
		if (std::time(NULL) - it->second.getCgi()._start > TIMEOUTCGI)
		{
			cgiDone = true;
			kill(it->second.getCgi()._pid, SIGKILL);
				it->second.setStatus(504);
		}
		else if (waitpid(it->second.getCgi()._pid, &status, WNOHANG) > 0)
		{
			cgiDone = true;
			char buffer[BUFSIZE];
			int ret = read(it->second.getCgi()._fdout[0], buffer, BUFSIZE - 1);
			if ((WIFEXITED(status) && WEXITSTATUS(status) != 0) || ret < 0)
				it->second.setStatus(502);
			else
			{
				buffer[ret] = '\0';
				std::string respbuff = buffer;
				it->second.getCgi()._respbody = buffer;
				it->second.setStatus(200);
			}
		}
		if (cgiDone)
		{
			close(it->second.getCgi()._fdout[0]);
			it->second.checkCGISHeader();
			Response resp(it->second);
			resp.buildResponse();
			if (it->second.getStatus() == 502 || it->second.getStatus() == 504)
				_fdtoServ[it->first]->setClientRequest(it->first, "close");
			else
				_fdtoServ[it->first]->setClientRequest(it->first, it->second.getHeaderField("Connection"));
			_fdtoServ[it->first]->setClientResponse(it->first,resp.getResponse());
			it->second.initRequest();
			setPoll(it->first, POLLOUT);
		}
	}
}
	