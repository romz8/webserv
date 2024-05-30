/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:33:49 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 05:44:11 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <vector>
# include <iostream>
# include <fstream>
# include <Utils.hpp>
# include <Defines.hpp>

class ServerConfig;
class LocationConfig;



class Parser {
	private:
		typedef void	(*parseSimpServ) (std::string, ServerConfig &);
		typedef parseSimpServ	parseSimpleSA[ N_SERVER_DIRECTIVES ];
		static parseSimpleSA _parseSimpleSA;
		typedef void	(*parseSimpLoc) (std::string, LocationConfig &);
		typedef parseSimpLoc	parseSimpleLA[ N_LOCATION_DIRECTIVES ];
		static parseSimpleLA _parseSimpleLA;
		std::string _file;
		std::string	_content;
		StrVector _allowed_directives;
		StrVector _directives;
		StrVector _simpleDirectives;
		StrVector _complexDirectives;

		Parser( void );
		void	init( void );
	
	public:
		Parser( int ac, char **av );
		~Parser( void );
		const std::string &	getFile( void );
		
		StrVector	getAD( void );
		std::string	getContent( void );
		
		void	parseLine( std::vector<ServerConfig> &vector, StrVector allowed_directives, std::string &content);
		void	getConfig( std::vector<ServerConfig> &vector, StrVector allowed_directives, std::string &content );
		void	parseDirective( std::string head, std::string body, std::vector<ServerConfig> &sserver, StrVector allowed_directives );

		void	getLocConfig( LocationConfig &location, StrVector allowed_directives, std::string &content );
		void	parseLocLine( LocationConfig &location, StrVector allowed_directives, std::string &content);
		void	parseDirectiveLoc( std::string head, std::string body, LocationConfig &location, StrVector allowed_directives );

	private:
		void	parseServer( std::string head, std::string body, std::vector<ServerConfig> &servers );
		void	parseLocation( std::string head, std::string body, std::vector<ServerConfig> &servers );
		
		void	getInfo( void );
		
		void	check_command( int ac, char **av );
		void	checkValidDirective( std::string name, StrVector allowed_directives );
		void	checkValidSeparator( std::string name, int type );
		
		void	parseComplex( std::string, std::string, std::vector<ServerConfig> &);
		
		void	parseSimpleServ( std::string, std::string, ServerConfig &, StrVector allowed_directives);
		void	parseSimpleLoc( std::string, LocationConfig &, StrVector allowed_directives );
};

#endif