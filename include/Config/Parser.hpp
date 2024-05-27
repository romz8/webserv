/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:33:49 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/27 13:03:03 by jsebasti         ###   ########.fr       */
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

class ServerConfig;

class Parser {
	private:
		std::string _file;
		std::string	_content;
		std::vector<std::string> allowed_directives;
		std::vector<std::string> _directives;
		std::vector<std::string> _simpleDirectives;
		std::vector<std::string> _complexDirectives;

		Parser( void );
		void	init( void );
	
	public:
		Parser( int ac, char **av );
		~Parser( void );
		const std::string &	getFile( void );
		std::vector<ServerConfig> &getConfig( std::vector<ServerConfig> &servers, std::vector<std::string> allowed_dirrectives );
		
		std::vector<std::string>	getAD( void );

		void	parseLine( std::vector<ServerConfig> &servers );

	private:
		void	getInfo( void );
		void	check_command( int ac, char **av );
		void	checkValidDirective( std::string name );
		void	checkValidSeparator( std::string name, int type );
};

#endif