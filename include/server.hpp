/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:31:46 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/22 20:15:46 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <poll.h>
# include "ServerConfig.hpp"
# include "Socket.hpp"
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "colors.h"




// typedef struct Config
// {
// 	std::string	host;
// 	std::string	port; //need to atoi : careful on overflow in conf file parsing
// 	std::string	serverName;
// 	std::string	root;
// 	std::string	index;
// 	std::string	hostName;
// 	std::map<int, std::string> errPageGlobal;
// 	std::vector<CgiConfig> cgiConf;
	
// 	// Location	loc; 	//later on std::vector<Location> locs;
// } Config;

// struct client
// {
// 	int fd;
// 	std::string request;
// 	std::string response;
// 	bool httpDone;
// };

class Server
{
private:
	int	_port;
	std::string	_root;
	std::string	_serverName;
	std::string	_hostName;
	std::string	_host; //is it corrrect to have hostName and host ? how is it done on José side
	size_t		_maxBodySize;
	std::vector<Location>	_locations; 
	sockaddr_in _servAddr;
	std::map<int, std::string> _errPageGlobal;
	Location _rootloc; //if no url match a location, use root location
	Socket _sock;
	std::vector<struct  pollfd> _fdSet;
	static const int	_timeout = 3;
	std::map<int , std::string> _clientRequest;
	std::map<int , std::string> _clientResponse;
	ServerConfig _conf;
	
	

	Socket socketFactory(const sockaddr_in& addr);

public:
	Server(const ServerConfig& conf);
	Server(const Server& src);
	Server& operator=(const Server& src);
	~Server();
	
	void 	_initServ();
	void	_initLocations(const std::vector<LocationConfig>& locations);
	std::string getRequest();
	std::string getResponse();
	//void	run();
	//void	handleConnection();
	int		acceptConnection(); //added for overwritting
	const Location* findLocationForRequest(const std::string& requestPath) const;
	static const sockaddr_in setServAddr(const ServerConfig& conf);
	void	addPollFd(int fd, short events);
	void	removePollFd(int fd);
	void	setPoll(pollfd& pfd, short events);
	void	readClient(pollfd& pfd);
	void	sendClient(pollfd& pfd);
	// void	readClient(int fd);
	// void	sendClient(int fd); 
	void	closeClient(int io_fd);
	void	handleError(const int io_socket, const int error);
	void	processRequest(const std::string& headeer, pollfd& pfd);
	int		getSocketInit()const;
	friend std::ostream& operator<<(std::ostream& os, const Server& serv);
};

std::ostream& operator<<(std::ostream& os, const Server& serv);

#endif

