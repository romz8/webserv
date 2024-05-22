/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:20:31 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/22 20:35:48 by rjobert          ###   ########.fr       */
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

Cluster::~Cluster(){}

std::vector<Server> Cluster::getServers() const
{
	return (_servers);
}

void Cluster::addServer(const ServerConfig& server)
{
	_servers.push_back(Server(server));
}

void Cluster::run()
{
	std::cout << "Cluster is running" << std::endl;
	
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		addPollFd(it->getSocketInit(), POLLIN, &(*it)); //add server socket to poll set ?? why &(*it)
		std::cout << "Server added to poll set" << std::endl;
	}
	std::cout << "Poll set" << std::endl;
	while (true)
	{
		int ret = poll(_fdSet.data(), _fdSet.size(), _timeout * 1000);
		if (ret < 0)
		{
			std::cerr << "Poll error" << std::endl;
			_fdSet.clear();
			for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
				addPollFd(it->getSocketInit(), POLLIN, &(*it));
		}
		else if (ret == 0)
			std::cout << GREEN "Waiting Connection ..." RESET << std::endl;
		else
		{
			//std::cout << "Poll returned" << std::endl;
			for (size_t i = 0; i < _fdSet.size(); ++i)
			{
				if (_fdSet[i].revents & POLLIN)
				{
					std::cout << "Pollin" << std::endl;
					std::map<int, Server*>::iterator it = _fdtoServ.find(_fdSet[i].fd);
					if (it != _fdtoServ.end())
						break; //handle later
					Server *serv = it->second;
					if (_fdSet[i].fd == serv->getSocketInit())
						{
							int newFd = serv->acceptConnection();
							if (newFd < 0)
								std::cerr << "Accept error" << std::endl;
							else
								addPollFd(newFd, POLLIN, serv);
						}
					else
						serv->readClient(_fdSet[i]);
				}
				else if (_fdSet[i].revents & POLLOUT)
				{
					std::map<int, Server*>::iterator it = _fdtoServ.find(_fdSet[i].fd);
					if (it != _fdtoServ.end())
						break; //handle later
					Server *serv = it->second;
					serv->sendClient(_fdSet[i]);
				}
			}
		}	
	}
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
 * Searches for and removes the specified file descriptor from the _fdSet, effectively
 * stopping any further monitoring of this descriptor in the server loop.
 * 
 * @param fd The file descriptor to remove.
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
	_fdtoServ.erase(fd);
}


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