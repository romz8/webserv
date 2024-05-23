/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:20:31 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/23 20:22:17 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster::Cluster(std::vector<ServerConfig> serverConfig) 
{
	_servers.clear();
	_fdSet.clear();
	_fdtoServ.clear();
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
	for (int i = 0; i < _fdSet.size(); ++i)
		close(_fdSet[i].fd);
	_servers.clear();
	_fdSet.clear();
	_fdtoServ.clear();
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
		addPollFd(it->getSocketInit(), POLLIN, &(*it)); 
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
 */
void Cluster::run()
{
	setUpServer();	
	
	while (true)
	{
		int ret = poll(_fdSet.data(), _fdSet.size(), _timeout * 1000);
		if (ret < 0)
		{
			_fdSet.clear();
			for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
				addPollFd(it->getSocketInit(), POLLIN, &(*it));
		}
		else if (ret == 0)
			std::cout << GREEN "Waiting Connection ..." RESET << std::endl;
		else
		{
			for (size_t i = 0; i < _fdSet.size(); ++i)
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
								removePollFd(_fdSet[i].fd);
							}
							else
								addPollFd(newFd, POLLIN, serv);
						}
					else
					{
						int ret = serv->readClient(_fdSet[i]);
						if (ret <= 0)
							removeClient(_fdSet[i].fd);
						else
							setPoll(_fdSet[i].fd, POLLOUT, serv);
					}
				}
				else if (_fdSet[i].revents & POLLOUT)
				{
					int ret = serv->sendClient(_fdSet[i]);
					if (ret < 0)
						removeClient(_fdSet[i].fd);
					else if (ret == 1)
						setPoll(_fdSet[i].fd, POLLIN, serv);
				}
				else if (_fdSet[i].revents & (POLLERR | POLLHUP | POLLNVAL))
					removeClient(_fdSet[i].fd);
				_fdSet[i].revents = 0;
			}
		}
		usleep(1000);
	}
}

/**
 * @brief Adds a file descriptor and associated events to the polling set.
 * 
 * The `addPollFd` function registers a new file descriptor along with the events it should be 
 * monitored for (e.g., `POLLIN`, `POLLOUT`) into the `_fdSet`, which is used by the `poll()` system call.
 * Then it maps the file descriptor to a `Server` instance to handle the events.
 */
void	Cluster::addPollFd(int fd, short events, Server* server)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	_fdSet.push_back(pfd);
	_fdtoServ[fd] = server;
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
void	Cluster::setPoll(int fd, short events, Server* server)
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
	
	removePollFd(fd);
	close(fd);
	_fdtoServ.erase(fd);
}