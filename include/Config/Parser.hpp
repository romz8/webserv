/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:33:49 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/28 19:59:53 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <vector>
# include <iostream>
# include <fstream>
# include <Utils.hpp>

# define N_SERVER_DIRECTIVES 8
# define N_LOCATION_DIRECTIVES 10
# define N_SIMPLE_DIRECTIVES 13
# define N_COMPLEX_DIRECTIVES 2
# define DIRECTIVES_NUM 15
# define SIMPLE 3
# define SERVER 0
# define LOCATION 1

class ServerConfig;
class LocationConfig;



class Parser {
	private:
		typedef void	(*parseSimpServ) (std::string, ServerConfig &);
		typedef parseSimpServ	parseSimpleSA[ N_SERVER_DIRECTIVES ];
		static parseSimpleSA _parseSimpleSA;
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
		int		checkValidDirective( std::string name, StrVector allowed_directives );
		void	checkValidSeparator( std::string name, int type );
		
		void	parseComplex( std::string, std::string, std::vector<ServerConfig> &);
		
		void	parseSimpleServ( std::string, ServerConfig &, StrVector allowed_directives);
		void	parseSimpleLoc( std::string, LocationConfig &, StrVector allowed_directives );
};

#endif