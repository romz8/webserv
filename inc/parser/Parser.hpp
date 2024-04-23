/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:33:49 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/18 22:37:03 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <ParseContent.hpp>
# include <ParseDirectives.hpp>
# include <Directives.hpp>
# include <Utils.hpp>
# include <iostream>
# include <fstream>

class Directives;

class Parser {
	private:
		Directives		*directives;
		ServersVector	server;
		std::string file;
		std::string	content;

		Parser( void );
	
	public:
		Parser( int ac, char **av );
		~Parser( void );
		const std::string &	getFile( void );
		Directives	*getDirectives( void ) ;

	private:
		void	getInfo( void );
		void	check_command(int ac, char **av);
};

#endif