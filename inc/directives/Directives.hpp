/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directives.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:11:20 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/26 09:28:23 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __DIRECTIVES_HPP__
# define __DIRECTIVES_HPP__

# include <DirectivesDefine.hpp>
# include <TypeDefines.hpp>
# include <Location.hpp>
# include <Server.hpp>
# include <Utils.hpp>

using namespace std;
using namespace SUtils;

class Directives {

	friend class Server;
	friend class ParseDirectives;
	friend class Location;

	private:
		string					def_err_page;
	private:
		string					root;
		unsigned int			port;
		UintStrMap				error_page;
		unsigned int			client_max_body_size;
		
		string					upload_store;
		bool					allow_upload;
		
		string					alias;
		
		IntStrPair				_return;
		

		StrMap					cgi;

		string					index;
		bool					autoindex;

		StrVector				allow_methods;
		
		StrVector				server_names;
		string					ip;
		
		ServersVector			servers;
		LocVector				locations;

		StrBoolMap				dirSet;
	
		void			cleanLocations( void );
		void			cleanServers( void );
	public:
		Directives( void );
		~Directives( void );
		Directives( const Directives & );
		Directives( const Directives &, int );
		Directives	&operator=( const Directives & );

		static Directives	*parseDirectives( const std::string & content );
		
		const string					&getIp( void ) const ;
		const unsigned int				&getPort( void ) const ;
		const string					&getRoot( void ) const ;
		const StrVector					&getSNames( void ) const ;
		const ServersVector				&getServers( void ) const ;
		const Server					*getServer( unsigned int ) const ;
		const Location					*getLocation( unsigned int ) const ;
		const UintStrMap::mapped_type	&getErrorPage( unsigned int ) const ;
		const UintStrMap				&getErrorPages( void ) const;
		const bool						&getAutoindex( void ) const;
		const string					&getUploadStore( void ) const;
		
		bool							errorPageSet( unsigned int ) const;
		bool							cgiSet( string ) const;
		
		void			addServer( Server * );
		void			addLocation( Location * );
		void			dupLocations( const LocVector & );
};

#endif