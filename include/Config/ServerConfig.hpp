/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 02:19:16 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/16 20:53:21 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ServerConfig_HPP__
# define __ServerConfig_HPP__

#include <iostream>
#include <Socket.hpp>
#include <Logs.hpp>

class Directives;
class LocationConfig;

// class ServerConfig
// {
// 	private:
// 		std::string	_host;
// 		std::string	_port;
// 		std::string	_root;
// 		std::string	_ServerName;
// 		std::string	_hostName;
// 		size_t		_maxBodySize;
// 		std::vector<Location>	_locations; 
// 		const sockaddr_in _servAddr;
// 	public:
// 		ServerConfigConfig( void );
// 		~ServerConfigConfig( void );
// 		ServerConfigConfig( ServerConfigConfig & );
// 		ServerConfigConfig	&operator=( ServerConfigConfig & );
// };
class ServerConfig
{
	private:
		Directives		*_d;
		Socket			_sock;
		Logs			_logs;
		struct sockaddr_in	_servAddr;
	public:
		ServerConfig( void );
		~ServerConfig( void );
		ServerConfig( ServerConfig & );
		ServerConfig	&operator=( ServerConfig & );

		UintStrMap::mapped_type	&getErrorPage( unsigned int ) const ;
		void					parse( std::string & );
		void					parse_ServerConfig_line( std::string &s_line );

		
		void					init_sock( void );
		void					setlisten( void );
		void					run( void ) const ;
		int						getFd( void ) const;
		
		//return the index of the coincidence to save it in a tmp map var to see which ServerConfig
		//has the stronger coincidence. (Maybe I move it to the directives to check with all the ServerConfigs).
		int						getStrongCoincidence( string ) const;
		Directives				*getDirectives( void ) const;
		LocationConfig				*getLocation( unsigned int ) const ;
};

#endif
