/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 02:19:16 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/22 23:28:42 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <string>
# include <vector>
# include <Directives.hpp>
# include <ParseContent.hpp>
# include <ParseDirectives.hpp>

class Server
{
	private:
		Directives		*directives;
	
	public:
		Server( void );
		~Server( void );
		Server( const Server & );
		Server	&operator=( const Server & );

		const UintStrMap::mapped_type	&getErrorPage( unsigned int ) const ;
		void							parse( std::string & );
		void							parse_server_line( std::string &s_line );

		const Location					*getLocation( unsigned int ) const ;
};

#endif
