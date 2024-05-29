/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:07:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/29 15:18:13 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSECONTENT_HPP__
# define __PARSECONTENT_HPP__

# include <iostream>
# include <fstream>
# include <limits>
# include <vector>

using namespace std;

// # define MAX_NUMERIC_LIMITS( t ) numeric_limits< t >::max()
// # define MAX_NUMERIC_LIMITS_STR( t ) to_string( MAX_NUMERIC_LIMITS( t ) )
// # define COMPARE_NBR_MAX_STR( s, t ) compareNumbersAsStrings( s, MAX_NUMERIC_LIMITS_STR( t ) )

# define N_SERVER_DIRECTIVES 8
# define N_LOCATION_DIRECTIVES 10
# define DIRECTIVES_NUM 15
# define N_SIMPLE_DIRECTIVES 13
# define N_COMPLEX_DIRECTIVES 2

class ServerConfig;

class ParseContent {
	private:
		ParseContent( void );


	public:
		~ParseContent( void );
		template <typename T>
		static void		save_root(std::string head, T &);
		static string	total_directives[ DIRECTIVES_NUM ];
		static string	location_directives[ N_LOCATION_DIRECTIVES ];
		static string	server_directives[ N_SERVER_DIRECTIVES ];
		static string	simple_directives[ N_SIMPLE_DIRECTIVES ];
		static string	complex_directives[ N_COMPLEX_DIRECTIVES ];
		// static int	parse_content( string &content );

		// static int	getServAllowDirectives( const string & );
		// static int	getLocAllowDirectives( const string & );

		// static StrBoolPair		_canRepeatDirectiveList[N_DIRECTIVES];
		// static StrBoolMap		canRepeatDirectiveList;
		// static void				checkValidIp( string );
		// static bool				checkIpSyntax( string );
		// static unsigned int		getMaskLimit( size_t );
		// static bool				checkValidRangeIpMask( string, size_t, size_t );
		// static void				checkValidPort( unsigned int );
		// static string			decompressIp( string );
		// static string			decompressBytes( string, size_t, size_t );
		// static bool				checkHttpPrefix( string );
		// static int				checkErrorCode( string );
};

# include <ParseDirectives.ipp>

#endif