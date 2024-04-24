/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:13:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/25 00:10:48 by jsebasti         ###   ########.fr       */
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
	make_pair<	const string, bool >( "listen", false ),
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

void	ParseContent::checkValidPort( unsigned int port ) {
	if (port > MAX_NUMERIC_LIMITS( unsigned short ))
		throw logic_error("Invalid port");
}

void	ParseContent::checkValidIp( string ip ) {
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

string	ParseContent::decompressIp( string ip ) {
	StrVector	args;
	string		decompress;

	split( args, ip, "." );
	for ( size_t i = 0; i < args.size(); i++ )
	{
		decompress += decompressBytes( args[ i ], i, args.size() );
		if ( i + 1 < args.size() )
			decompress += ".";
	}
	return ( decompress );
}

string	ParseContent::decompressBytes( string compressed, size_t pos, size_t size ) {
	string		decompress;
	unsigned int	num;

	decompress = compressed;
	if ( pos == ( size - 1 ) )
	{
		decompress = "";
		num = stoui( compressed );
		for ( int i = 4 - size; i >= 0; i-- )
		{
			decompress += long_to_string( ( num >> ( i * 8 ) ) & 0xFF );
			if ( i - 1 >= 0 )
				decompress += ".";
		}
	}
	return ( decompress );
}

unsigned int	ParseContent::getMaskLimit( size_t pos ) {
	return ( MAX_NUMERIC_LIMITS( unsigned int ) >> (pos * 8) );
}

bool	ParseContent::checkValidRangeIpMask( string num, size_t pos, size_t size ) {
	string limit;

	limit = long_to_string((pos == (size - 1)) ? ParseContent::getMaskLimit(pos) : 0xFF);
	if ( compareNumbersAsStrings( num, limit ) > 0 )
		return ( false );
	return (true);
}

void	ParseContent::checkFolder( string _dir ) {
	if (_dir[_dir.length() - 1] != '/')
		_dir.append("/");
	
}