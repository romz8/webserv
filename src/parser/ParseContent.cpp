/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:13:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 13:40:00 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ParseContent.hpp>

using namespace std;

ParseContent::ParseContent( void ) {
	return ;
}

ParseContent::~ParseContent( void ) {
	return ;
}

string ParseContent::server_directives[ N_SERVER_DIRECTIVES ] = {
	"root",
	"listen",
	"error_page",
	"server_name",
	"client_max_body_size",
	"upload_store",
	"autoindex",
	"location",
};


string ParseContent::location_directives[ N_LOCATION_DIRECTIVES ] = {
	"root",
	"error_page",
	"upload_store",
	"allow_update",
	"index",
	"alias",
	"autoindex",
	"return",
	"allow_methods",
	"cgi",
};

string ParseContent::total_directives[ N_DIRECTIVES ] = {
	"root",
	"listen",
	"error_page",
	"server_name",
	"client_max_body_size",
	"upload_store",
	"allow_update",
	"index",
	"alias",
	"autoindex",
	"return",
	"allow_methods",
	"cgi",
	"location",
	"server"
};

StrBoolPair	ParseContent::_canRepeatDirectiveList[ N_DIRECTIVES ] = {
	make_pair<	const string, bool >( "root", false ),
	make_pair<	const string, bool >( "listen", true ),
	make_pair<	const string, bool >( "server_name", true ),
	make_pair<	const string, bool >( "error_page", true ),
	make_pair<	const string, bool >( "client_max_body_size", false ),
	make_pair<	const string, bool >( "upload_store", false ), 
	make_pair<	const string, bool >( "index", true ),
	make_pair<	const string, bool >( "autoindex", false ),
	make_pair<	const string, bool >( "alias", false ),
	make_pair<	const string, bool >( "return", false ),
	make_pair<	const string, bool >( "allow_methods", false ),
	make_pair<	const string, bool >( "cgi", true ),
	make_pair<	const string, bool >( "server", true ),
	make_pair<	const string, bool >( "location", true )
};

StrBoolMap	ParseContent::canRepeatDirectiveList( ParseContent::_canRepeatDirectiveList, ParseContent::_canRepeatDirectiveList + N_DIRECTIVES );

int	ParseContent::getServAllowDirectives( const string & directive ) {
	for (int i = 0; i < N_SERVER_DIRECTIVES; i++)
	{
		int diff = directive.compare(ParseContent::server_directives[i]);
		if (!diff)
			return (i);
	}
	return (ERROR);
}

int	ParseContent::getLocAllowDirectives( const string & directive ) {
	for (int i = 0; i < N_LOCATION_DIRECTIVES; i++)
	{
		int diff = directive.compare(ParseContent::location_directives[i]);
		if (!diff)
			return (i);
	}
	return (ERROR);
}

string	&ParseContent::checkValidIp( string ip ) {
	StrVector	masks;
	
	if (checkIpSyntax(ip) == false)
		throw logic_error("Invalid syntax in IP");
	split(masks, ip, ".");
	if (masks.size() > 4)
		throw logic_error("Invalid IP");
	for ( size_t i = 0; i < masks.size(); i++ )
	{
		if ( checkValidRangeIpMask( masks[ i ], i, masks.size() ) == false )
			throw logic_error("Invalid IP");
	}
}

bool	ParseContent::checkIpSyntax( string ip ) {
	size_t	pos;

	pos = ip.find_first_not_of( IP_VALID_CHARS );
	if ( pos != string::npos || ip[ 0 ] == '.' )
		return ( false );
	pos = 0;
	while ( pos != string::npos )
	{
		pos = ip.find_first_of( "." );
		if ( pos != string::npos )
		{
			if ( ip[ pos + 1 ] == '\0' || ip[ pos + 1 ] == '.' )
				return ( false );
			ip.erase( 0, pos + 1 );
		}
	}
	return (true);
}

bool	ParseContent::checkValidRangeIpMask( string mask, size_t pos, size_t size ) {
	
}