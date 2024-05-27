/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:07:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/23 15:28:47 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSECONTENT_HPP__
# define __PARSECONTENT_HPP__

# include <Utils.hpp>
# include <iostream>
# include <fstream>
# include <limits>

using namespace std;
using namespace SUtils;

# define MAX_NUMERIC_LIMITS( t ) numeric_limits< t >::max()
# define MAX_NUMERIC_LIMITS_STR( t ) to_string( MAX_NUMERIC_LIMITS( t ) )
# define COMPARE_NBR_MAX_STR( s, t ) compareNumbersAsStrings( s, MAX_NUMERIC_LIMITS_STR( t ) )

class ParseContent {
	private:
		ParseContent( void );
	
	public:
		~ParseContent( void );
		
		static int	parse_content( std::string &content );

		static int	getServAllowDirectives( const string & );
		static int	getLocAllowDirectives( const string & );

		static StrBoolPair		_canRepeatDirectiveList[N_DIRECTIVES];
		static StrBoolMap		canRepeatDirectiveList;
		static void				checkValidIp( string );
		static bool				checkIpSyntax( string );
		static unsigned int		getMaskLimit( size_t );
		static bool				checkValidRangeIpMask( string, size_t, size_t );
		static void				checkValidPort( unsigned int );
		static string			decompressIp( string );
		static string			decompressBytes( string, size_t, size_t );
		static bool				checkHttpPrefix( string );
		static int				checkErrorCode( string );
};

#endif