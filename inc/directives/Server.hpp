/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 02:19:16 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 21:25:20 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <ParseContent.hpp>
# include <ParseDirectives.hpp>
# include <Socket.hpp>

class Server
{
	private:
		Directives		*_d;
		Socket			_sock;
		struct sockaddr_in	_servAddr;
	public:
		Server( void );
		~Server( void );
		Server( const Server & );
		Server	&operator=( const Server & );

		const UintStrMap::mapped_type	&getErrorPage( unsigned int ) const ;
		void							parse( std::string & );
		void							parse_server_line( std::string &s_line );

		const Location					*getLocation( unsigned int ) const ;
		
		//return the index of the coincidence to save it in a tmp map var to see which server
		//has the stronger coincidence. (Maybe I move it to the directives to check with all the servers).
		int								getStrongCoincidence( string ) const;
		void							init_sock( void );
		void							run( void ) const ;
};

#endif
