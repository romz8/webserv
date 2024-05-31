/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 13:12:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 02:45:47 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <sstream>
# include <vector>

# define MAX_NUMERIC_LIMITS( t ) numeric_limits<t>::max()
# define MAX_NUMERIC_LIMITS_STR( t ) to_string( MAX_NUMERIC_LIMITS( t ) )
# define COMPARE_NBR_MAX_STR( s, t ) compareNumbersAsStrings( s, MAX_NUMERIC_LIMITS_STR( t ) )

# define ISNUM "0123456789"
# define IPVALID ".0123456789"
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
	template <typename T>
	int	easyfind( typename T::iterator begin, typename T::iterator end, std::string toFind );
	template <typename T>
	typename T::iterator	it_easyfind( const T container, std::string toFind );
	// std::string		long_to_string( long int );
	// int				check_brackets( StrVector & content, int n_line );
	// void			parse_line( const std::string & , int );
	// unsigned int	stoui( const std::string &num );
}

namespace std {
	template <typename T>
	string	to_string( const T &n );
	template <typename T>
	T		stoui( string );
	
	int		ft_stoi( string );
}

std::string			deleteExtraZeros( const std::string );
int				compareNumbersAsStrings( const std::string , const std::string );
std::string		trim_left( std::string );
std::string		trim_right( std::string );
void            split_pair( const std::string str, std::string &head, std::string &body );
void			checkValidIp(std::string);
bool			checkIpSyntax( std::string ip );
# include <Utils.ipp>

#endif
