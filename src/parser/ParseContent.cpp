/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:13:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 10:33:24 by jsebasti         ###   ########.fr       */
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