/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:13:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 18:22:04 by jsebasti         ###   ########.fr       */
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
	"return",
	"autoindex",
	"cgi"
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
	"allow_methods",
	"return",
	"cgi"
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
	"return",
	"autoindex",
	"allow_methods",
	"cgi",
	"location",
	"server"
};

const pair<const std::string, bool>	ParseContent::canRepeat[DIRECTIVES_NUM] = {
	make_pair<	const string, bool >( "root", false ),
	make_pair<	const string, bool >( "listen", false ),
	make_pair<	const string, bool >( "server_name", true ),
	make_pair<	const string, bool >( "error_page", true ),
	make_pair<	const string, bool >( "client_max_body_size", false ),
	make_pair<	const string, bool >( "upload_store", false ), 
	make_pair<	const string, bool >( "index", false ),
	make_pair<	const string, bool >( "autoindex", false ),
	make_pair<	const string, bool >( "return", false ),
	make_pair<	const string, bool >( "allow_methods", false ),
	make_pair<	const string, bool >( "cgi", true ),
	make_pair<	const string, bool >( "server", true ),
	make_pair<	const string, bool >( "location", true )
};

const map<const std::string, bool>	ParseContent::canRepeatList( ParseContent::canRepeat, ParseContent::canRepeat + DIRECTIVES_NUM);

//			SERVER CONFIG DIRECTIVES 

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
	config.setListen(port, ip);
}

void		ParseContent::save_server_name(string head, ServerConfig &config) {
	StrVector	data;
	SUtils::split(data, head, ISSPACE);
	
	if (data.size() < 2)
		throw logic_error("Invalid number of arguments for " + data[0]);
	for (size_t i = 1; i < data.size(); i++)
		config.setServerName(data[i]);
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

//		LOCATIONS CONFIG DIRECTIVES

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


void	ParseContent::save_allow_methods(string head, LocationConfig &config) {
	StrVector data;
	string value;
	StrVector allowedmethods;
	SUtils::split(data, head, ISSPACE);
	if (data.size() < 2 || data.size() > 4)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	for (size_t i = 1; i < data.size(); i++)
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

void	ParseContent::save_return(string head, LocationConfig &config) {
	StrVector data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	config.setAlias(data[1]);
}

//		END FUNCTIONS -----------------------------------
