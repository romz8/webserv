/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:13:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 06:18:31 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Utils.hpp>
#include <ParseContent.hpp>
#include <ServerConfig.hpp>
#include <Parser.hpp>

ParseContent::ParseContent( void ) {
	return ;
}

ParseContent::~ParseContent( void ) {
	return ;
}

string	ParseContent::server_directives[ N_SERVER_DIRECTIVES ] = {
	"root",
	"listen",
	"server_name",
	"client_max_body_size",
	"autoindex",
	"error_page",
	"location",
};

string	ParseContent::simple_directives[ N_SIMPLE_DIRECTIVES ] = {
	"listen",
	"server_name",
	"client_max_body_size",
	"root",
	"error_page",
	"upload_dir",
	"allow_upload",
	"allow_methods",
	"index",
	"alias",
	"autoindex",
	"cgi"
	// "return",
};

string	ParseContent::complex_directives[ N_COMPLEX_DIRECTIVES ] = {
	"server",
	"location"
};

string	ParseContent::location_directives[ N_LOCATION_DIRECTIVES ] = {
	"root",
	"error_page",
	"autoindex",
	"upload_dir",
	"allow_upload",
	"index",
	"alias",
	"allow_methods",
	"cgi"
	// "return",
};

string	ParseContent::total_directives[ DIRECTIVES_NUM ] = {
	"root",
	"listen",
	"error_page",
	"server_name",
	"client_max_body_size",
	"upload_dir",
	"allow_upload",
	"index",
	"alias",
	"autoindex",
	"allow_methods",
	"cgi",
	"location",
	"server"
	// "return",
};

void	ParseContent::save_listen(string head, ServerConfig &config) {
	StrVector line;
	SUtils::split(line, head, ISSPACE);
	if (line.size() != 2)
		throw logic_error("Invalid number of arguments for " + line[0]);
	string data = line[1];
	size_t pos = data.find_first_of(":");
	int port;
	if (pos == string::npos)
	{
		port = ft_stoi(data);
		if (port < 0 || port > USHRT_MAX)
			throw logic_error("Invalid port " + data);
		config.setPort(atoi(data.c_str()));
		return ;
	}
	config.setHost(data);
	string ip = data.substr(0, pos);
	string s_port = data.substr(pos, string::npos);
	if (s_port.empty() || s_port.length() == 1)
		throw logic_error("Should give a port for " + ip);
	s_port = s_port.substr(1, string::npos);
	port = std::stoui<int>(s_port);
	if (port < 0 || port > USHRT_MAX)
			throw logic_error("Invalid port " + s_port);
	checkValidIp(ip);
	config.setPort(port);
	config.setHostName(ip);
}

void		ParseContent::save_server_name(string head, ServerConfig &config) {
	StrVector	data;
	SUtils::split(data, head, ISSPACE);
	
	if (data.size() != 2)
		throw logic_error("Invalid number of arguments for " + data[0]);
	config.setServerName(data[1]);
}

void		ParseContent::save_client_max_body_size(string head, ServerConfig &config) {
	StrVector line;
	SUtils::split(line, head, ISSPACE);
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1];
	
	size_t pos_first_letter = value.find_first_not_of(ISNUM);
	size_t num = stoui<size_t>(value.substr(0, pos_first_letter));
	string letter;

	if (pos_first_letter != string::npos) {
		letter = value.substr(pos_first_letter, value.length());
	} 
	else {
		if (num > MAX_BODY_SIZE)
			throw logic_error("Value " + value + " invalid, the value is limmited to 10000000000 bytes");
		config.setClientMaxBodySize(num);
		return ;
	}
	
	if (letter.length() > 1 || letter.find_first_of("kmgKMG") == string::npos)
		throw logic_error("Value " + value + " invalid.");
	
	if (value[pos_first_letter] == 'k' || value[pos_first_letter] == 'K')
	{
		if (num > MAX_BODY_SIZE / 1000)
			throw logic_error("Value " + value + " invalid, the value is limmited to 10000000k");
		config.setClientMaxBodySize(num * 1000);
	}
	else if (value[pos_first_letter] == 'm' || value[pos_first_letter] == 'M')
	{
		if (num > MAX_BODY_SIZE / 1000000)
			throw logic_error("Value " + value + " invalid, the value is limmited to 10000m");
		config.setClientMaxBodySize(num * 1000000);
	}
	else if (value[pos_first_letter] == 'g' || value[pos_first_letter] == 'G')
	{
		if (num > MAX_BODY_SIZE / 1000000000)
			throw logic_error("Value " + value + " invalid, the value is limmited to 10g");
		config.setClientMaxBodySize(num * 1000000000);
	}
}

void	ParseContent::save_upload_dir(string head, LocationConfig &config) {
	StrVector line;
	SUtils::split(line, head, ISSPACE);
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1];
	config.setUploadDir(value);
}

void	ParseContent::save_allow_upload(string head, LocationConfig &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	std::string boolean = data[1];
	for (size_t i = 0; i < boolean.length(); i++)
		boolean[i] = tolower(boolean[i]);
	if (!boolean.compare("false"))
		config.setAllowUpload(false);
	else if (!boolean.compare("true"))
		config.setAllowUpload(true);
	else
		throw std::logic_error("Invalid argument \"" + boolean + "\" expected: true|false");
}

void	ParseContent::save_index(string head, LocationConfig &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	config.setIndex(data[1]);
}

void	ParseContent::save_alias(string head, LocationConfig &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	// if (config.isSet("root"))
	// 	throw std::logic_error("Cannot initializate alias. Root setted previously");
	config.setAlias(data[1]);
}

void	ParseContent::save_allow_methods(string head, LocationConfig &config) {
	StrVector data;
	string value;
	StrVector allowedmethods;
	SUtils::split(data, head, ISSPACE);
	if (data.size() < 2 || data.size() > 4)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	for (int i = 1; i < data.size(); i++)
	{
		if (i != data.size() - 1 && data[i].find_first_of(";") != string::npos)
			throw logic_error("Unexpected token ; in value " + data[i]);
		else if (i == data.size() - 1)
			value = data[i].substr(0, data[i].find_first_of(";"));
		else
			value = data[i];
		if (value.compare("POST") && value.compare("GET") && value.compare("DELETE"))
			throw logic_error("Value \"" + value + "\" not valid for allow_methods");
		allowedmethods.push_back(value);
	}
	config.setAllowedMethods(allowedmethods);
}

void	ParseContent::save_cgi(string head, LocationConfig &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 3)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	config.addCgiConfig(CgiConfig(data[1], data[2]));
	
}

// void	ParseContent::save_return(string head, LocationConfig &config) {
// 	StrVector data;
// 	SUtils::split(data, head, ISSPACE);
// 	if (data.size() != 2)
// 		throw std::logic_error("Invalid number of arguments for " + data[0]);
// 	config.setReturn(data[1]);
// }

// StrBoolPair	ParseContent::_canRepeatDirectiveList[ N_DIRECTIVES ] = {
// 	make_pair<	const string, bool >( "root", false ),
// 	make_pair<	const string, bool >( "listen", false ),
// 	make_pair<	const string, bool >( "server_name", true ),
// 	make_pair<	const string, bool >( "error_page", true ),
// 	make_pair<	const string, bool >( "client_max_body_size", false ),
// 	make_pair<	const string, bool >( "upload_store", false ), 
// 	make_pair<	const string, bool >( "index", true ),
// 	make_pair<	const string, bool >( "autoindex", false ),
// 	make_pair<	const string, bool >( "alias", false ),
// 	make_pair<	const string, bool >( "return", false ),
// 	make_pair<	const string, bool >( "allow_methods", false ),
// 	make_pair<	const string, bool >( "cgi", true ),
// 	make_pair<	const string, bool >( "server", true ),
// 	make_pair<	const string, bool >( "location", true )
// };

// StrBoolMap	ParseContent::canRepeatDirectiveList( ParseContent::_canRepeatDirectiveList, ParseContent::_canRepeatDirectiveList + N_DIRECTIVES );

// int	ParseContent::getServAllowDirectives( const string & directive ) {
// 	for (int i = 0; i < N_SERVER_DIRECTIVES; i++)
// 	{
// 		int diff = directive.compare(ParseContent::server_directives[i]);
// 		if (!diff)
// 			return (i);
// 	}
// 	return (ERROR);
// }

// int	ParseContent::getLocAllowDirectives( const string & directive ) {
// 	for (int i = 0; i < N_LOCATION_DIRECTIVES; i++)
// 	{
// 		int diff = directive.compare(ParseContent::location_directives[i]);
// 		if (!diff)
// 			return (i);
// 	}
// 	return (ERROR);
// }

// void	ParseContent::checkValidPort( unsigned int port ) {
// 	if (port > MAX_NUMERIC_LIMITS( unsigned short ))
// 		throw logic_error("Invalid port");
// }

// void	ParseContent::checkValidIp( string ip ) {
// 	StrVector	masks;
	
// 	if (checkIpSyntax(ip) == false)
// 		throw logic_error("Invalid syntax in IP " + ip);
// 	split(masks, ip, ".");
// 	if (masks.size() > 4)
// 		throw logic_error("Invalid IP");
// 	for ( size_t i = 0; i < masks.size(); i++ )
// 	{
// 		if ( checkValidRangeIpMask( masks[ i ], i, masks.size() ) == false )
// 			throw logic_error("Invalid IP");
// 	}
// }

// bool	ParseContent::checkIpSyntax( string ip ) {
// 	size_t	pos;

// 	pos = ip.find_first_not_of( IP_VALID_CHARS );
// 	if ( pos != string::npos || ip[ 0 ] == '.' )
// 		return ( false );
// 	pos = 0;
// 	while ( pos != string::npos )
// 	{
// 		pos = ip.find_first_of( "." );
// 		if ( pos != string::npos )
// 		{
// 			if ( ip[ pos + 1 ] == '\0' || ip[ pos + 1 ] == '.' )
// 				return ( false );
// 			ip.erase( 0, pos + 1 );
// 		}
// 	}
// 	return (true);
// }

// string	ParseContent::decompressIp( string ip ) {
// 	StrVector	args;
// 	string		decompress;

// 	split( args, ip, "." );
// 	for ( size_t i = 0; i < args.size(); i++ )
// 	{
// 		decompress += decompressBytes( args[ i ], i, args.size() );
// 		if ( i + 1 < args.size() )
// 			decompress += ".";
// 	}
// 	return ( decompress );
// }

// string	ParseContent::decompressBytes( string compressed, size_t pos, size_t size ) {
// 	string		decompress;
// 	unsigned int	num;

// 	decompress = compressed;
// 	if ( pos == ( size - 1 ) )
// 	{
// 		decompress = "";
// 		num = stoui( compressed );
// 		for ( int i = 4 - size; i >= 0; i-- )
// 		{
// 			decompress += long_to_string( ( num >> ( i * 8 ) ) & 0xFF );
// 			if ( i - 1 >= 0 )
// 				decompress += ".";
// 		}
// 	}
// 	return ( decompress );
// }

// unsigned int	ParseContent::getMaskLimit( size_t pos ) {
// 	return ( MAX_NUMERIC_LIMITS( unsigned int ) >> (pos * 8) );
// }

// bool	ParseContent::checkValidRangeIpMask( string num, size_t pos, size_t size ) {
// 	string limit;

// 	limit = long_to_string((pos == (size - 1)) ? ParseContent::getMaskLimit(pos) : 0xFF);
// 	if ( compareNumbersAsStrings( num, limit ) > 0 )
// 		return ( false );
// 	return (true);
// }

// bool	ParseContent::checkHttpPrefix( string uri ) {
// 	string http = HTTP_PREFIX;
// 	string https = HTTPS_PREFIX;
	
// 	if (!uri.compare(0, http.length(), http))
// 		return (true);
// 	if (!uri.compare(0, https.length(), https))
// 		return (true);
// 	return (false);
// }

// int	ParseContent::checkErrorCode( string code ) {
// 	if (code.find_first_not_of(ISNUM) != string::npos)
// 		return (-1);
// 	int _code = atoi(code.c_str());
// 	if (_code < 0 || _code > 999)
// 		return (-1);
// 	return (_code);
// }