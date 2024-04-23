/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:07:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 20:03:42 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSECONTENT_HPP__
# define __PARSECONTENT_HPP__

# include <Server.hpp>
# include <DirectivesDefine.hpp>
# include <Utils.hpp>
# include <iostream>
# include <limits>

using namespace std;
using namespace SUtils;

# define MAX_NUMERIC_LIMITS( t ) numeric_limits< t >::max()

class ParseContent {
	private:
		ParseContent( void );
	
	public:
		~ParseContent( void );
		
		static int	getServAllowDirectives( const string & );
		static int	getLocAllowDirectives( const string & );

		static string			server_directives[N_SERVER_DIRECTIVES];
		static string			location_directives[N_LOCATION_DIRECTIVES];
		static string			total_directives[N_DIRECTIVES];
		static StrBoolPair		_canRepeatDirectiveList[N_DIRECTIVES];
		static StrBoolMap		canRepeatDirectiveList;
		static void				checkValidIp( string );
		static bool				checkIpSyntax( string );
		static unsigned int		getMaskLimit( size_t );
		static bool				checkValidRangeIpMask( string, size_t, size_t );
		static void				checkValidPort( unsigned int );
		static string			decompressIp( string );
		static string			decompressBytes( string, size_t, size_t );
};

#endif