/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:18:22 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/21 16:24:38 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"

class Cluster
{
private:
	std::vector<ServerConfig> _servers;

public:
	Cluster();
	Cluster(const Cluster& src);
	Cluster& operator=(const Cluster& src);
	~Cluster();

	std::vector<ServerConfig> getServers() const;
	void addServer(const ServerConfig& server);
	void	run();
		
};

# endif