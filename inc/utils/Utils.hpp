/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 13:12:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/22 23:27:39 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <TypeDefines.hpp>
# include <Defines.hpp>

namespace SUtils {

StrVector             &split( StrVector & v, std::string strArr, std::string delimiter );
std::string	        &treat_comments( std::string & str );
std::string		    trim( std::string );
int				    check_brackets( StrVector & content, int n_line );
void		    	parse_line( const std::string & , int );
unsigned int	    stoui( const std::string &num );

}
std::string		trim_left( std::string );
std::string		trim_right( std::string );
#endif