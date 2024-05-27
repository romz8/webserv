/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:33:49 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/25 18:44:01 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <vector>
# include <iostream>
# include <fstream>
# include <Utils.hpp>

class ServerConfig;

class Parser {
	private:
		std::string _file;
		std::string	_content;
		std::vector<std::string> allowed_directives;

		Parser( void );
	
	public:
		Parser( int ac, char **av );
		~Parser( void );
		const std::string &	getFile( void );
		std::vector<ServerConfig> &getConfig( std::vector<ServerConfig> &servers );
		
		void	parseLine( std::vector<ServerConfig> &servers );

	private:
		void	getInfo( void );
		void	check_command(int ac, char **av);
		void	checkValidDirective( std::string name );
};

#endif