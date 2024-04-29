/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:31:46 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/29 16:16:18 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# include "Socket.hpp"
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "colors.h"

# define MAX_BODY_SIZE 1000000

typedef struct Config
{
	std::string	host;
	std::string	port; //need to atoi : careful on overflow in conf file parsing
	std::string	serverName;
	std::string	root;
	std::string	index;
	std::string	hostName;
	std::map<int, std::string> errPageGlobal;
	std::vector<CgiConfig> cgiConf;
	// Location	loc; 	//later on std::vector<Location> locs;
} Config;


class Server
{
private:
	std::string	_host;
	std::string	_port;
	std::string	_root;
	std::string	_serverName;
	std::string	_hostName;
	size_t		_maxBodySize;
	std::vector<Location>	_locations; 
	const sockaddr_in _servAddr;
	std::map<int, std::string> _errPageGlobal;
	Location _rootloc; //if no url match a location, use root location
	Socket _sock;
	
	Server(const Server& src);
	Server& operator=(const Server& src);

	Socket socketFactory(const sockaddr_in& addr);

public:
	Server(const Config& conf);
	~Server();
	
	void 	_initServ();
	std::string getRequest();
	std::string getResponse();
	void	run();
	void	handleConnection();
	const Location* findLocationForRequest(const std::string& requestPath) const;
	static const sockaddr_in setServAddr(const Config& conf);
};

#endif

