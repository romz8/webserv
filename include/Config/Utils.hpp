/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 13:12:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 18:04:12 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <TypeDefines.hpp>
# include <Defines.hpp>
# include <sstream>

namespace SUtils {

StrVector		&split( StrVector & v, std::string strArr, std::string delimiter );
std::string		&treat_comments( std::string & str );
std::string		long_to_string( long int );
std::string		trim( std::string );
int				check_brackets( StrVector & content, int n_line );
void			parse_line( const std::string & , int );
unsigned int	stoui( const std::string &num );
string			deleteExtraZeros( const string );
int				compareNumbersAsStrings( const string , const string );
}
namespace std
{
    template <typename T>
	string	to_string(const T &);
}


std::string		trim_left( std::string );
std::string		trim_right( std::string );
#endif