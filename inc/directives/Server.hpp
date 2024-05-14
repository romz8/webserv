/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 02:19:16 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 17:30:38 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include <iostream>
#include <Socket.hpp>
#include <Logs.hpp>

class Directives;
class Location;

class Server
{
	private:
		Directives		*_d;
		Socket			_sock;
		Logs			_logs;
		struct sockaddr_in	_servAddr;
	public:
		Server( void );
		~Server( void );
		Server( Server & );
		Server	&operator=( Server & );

		UintStrMap::mapped_type	&getErrorPage( unsigned int ) const ;
		void					parse( std::string & );
		void					parse_server_line( std::string &s_line );

		
		void					init_sock( void );
		void					setlisten( void );
		void					run( void ) const ;
		int						getFd( void ) const;
		
		//return the index of the coincidence to save it in a tmp map var to see which server
		//has the stronger coincidence. (Maybe I move it to the directives to check with all the servers).
		int						getStrongCoincidence( string ) const;
		Directives				*getDirectives( void ) const;
		Location				*getLocation( unsigned int ) const ;
};

#endif
