/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directives.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:11:20 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/16 20:56:07 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __DIRECTIVES_HPP__
# define __DIRECTIVES_HPP__

# include <DirectivesDefine.hpp>
# include <TypeDefines.hpp>
# include <LocationConfig.hpp>
# include <ServerConfig.hpp>
# include <ParseContent.hpp>
# include <ParseDirectives.hpp>
# include <Utils.hpp>
# include <Logs.hpp>

using namespace std;
using namespace SUtils;

class Server;

class Directives {

	friend class Server;
	friend class ParseDirectives;
	friend class Location;

	private:
		string					def_err_page;
		string					def_serv_name;
		Logs					logs;

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
		Directives( Directives & );
		Directives( Directives &, int );
		Directives	&operator=( Directives & );
	
		static Directives	*parseDirectives( const std::string & content );
	
		bool							errorPageSet( unsigned int ) const;
		bool							cgiSet( string ) const;
		
		void			addServerConfig( Server * );
		void			addLocation( Location * );
		void			dupLocations( const LocVector & );
		void			setServersConfigToListen( void );
	
	public :
		const string					&getIp( void ) const ;
		const unsigned int				&getPort( void ) const ;
		const string					&getRoot( void ) const ;
		const StrVector					&getSNames( void ) const ;
		ServersVector					&getServers( void ) ;
		ServerConfig					*getServer( unsigned int ) const ;
		Location						*getLocation( unsigned int ) const ;
		UintStrMap::mapped_type			&getErrorPage( unsigned int ) ;
		UintStrMap						&getErrorPages( void );
		const bool						&getAutoindex( void ) const;
		const string					&getUploadStore( void ) const;
		const string					&getReturnValue( void ) const;
		const int						&getReturnCode( void ) const;
		const StrMap					&getCgiMap( void ) const;
		const string					getCgiExe( string ) const;
};		

#endif