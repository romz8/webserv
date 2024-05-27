/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:18:22 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/27 12:35:33 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <poll.h>
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include "Server.hpp"
# include "LocationConfig.hpp"

class Cluster
{
private:
	std::vector<Server> _servers;
	std::vector<struct  pollfd> _fdSet;
	std::map<int, Server*> _fdtoServ;
	std::map<int, Request> _fdtoReq;
	static const int	_timeout = 3;

public:
	Cluster(std::vector<ServerConfig> servers);
	Cluster(const Cluster& src);
	Cluster& operator=(const Cluster& src);
	~Cluster();

	std::vector<Server> getServers() const;
	void 	addServer(const ServerConfig& server);
	void	setUpServer();
	void	addPollFd(int fd, short events, Server* server);
	void	removePollFd(int fd);
	void	setPoll(int fd, short events, Server* server);
	void	removeClient(int fd);
	void	run();
};

# endif