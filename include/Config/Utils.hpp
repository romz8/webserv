/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 13:12:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/28 12:17:35 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <sstream>
# include <vector>

# define ISSPACE "\t\n\v\f\r "
# define NOT_SEPARATOR -1
# define COMMENT 1
# define BRACET 2
# define SEMICOLON 3

# define SERVER 0
# define LOCATION 1

typedef std::vector<std::string>    StrVector;

namespace SUtils {
	StrVector		&split( StrVector & v, std::string strArr, std::string delimiter );
	std::string		&treat_comments( std::string & str );
	std::string		trim( std::string );
	int             get_pair( std::string &content, std::string &head, std::string &body);
	size_t	        checkBracets( std::string content );
	void			set_allowed_directives( StrVector &allowed_methods, int flag );
	// std::string		long_to_string( long int );
	// int				check_brackets( StrVector & content, int n_line );
	// void			parse_line( const std::string & , int );
	// unsigned int	stoui( const std::string &num );
	// string			deleteExtraZeros( const string );
	// int				compareNumbersAsStrings( const string , const string );
}


std::string		trim_left( std::string );
std::string		trim_right( std::string );
void            split_pair( const std::string str, std::string &head, std::string &body );

# include <Utils.ipp>

#endif