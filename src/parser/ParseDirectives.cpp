/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 19:30:00 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/26 09:41:00 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <ParseDirectives.hpp>

using namespace SUtils;
using namespace std;

ParseDirectives::ParseDirectives( void ) {
	
}

ParseDirectives::~ParseDirectives( void ) {
	
}

int	ParseDirectives::parseServer( Directives *d, StrVector & content, int n_line ) {
	int last = check_brackets(content, n_line);
	StrVector line;
	string content_parsed;
	Server *serv = new Server;

	try {
		serv->parse_server_line(content[n_line]);
		for (int i = n_line; i < last; i++)
		{
			string treated_line = treat_comments((content[i]));
			parse_line(treated_line, i + 1);
			split(line, content[i], " \t");
			for (size_t j = 0; j < line.size(); j++)
				content_parsed += line[j] + " ";
			line.clear();
			treated_line.clear();
			content_parsed += "\n";
		}
		serv->parse(content_parsed);
		d->addServer(serv);
		d->dirSet["server"] = true;
	}
	catch (exception &e)
	{
		delete serv;
		throw logic_error(e.what());
	}
	return (last);
}

int	ParseDirectives::parseLocation( Directives *d, StrVector & content, int n_line) {
	int last = check_brackets(content, n_line);
	StrVector line;
	string content_parsed;
	string treated_line;
	Location *loc = new Location;

	try {
		loc->parse_location_line(content[n_line]);
		for (int i = n_line; i < last; i++)
		{
			string treated_line = treat_comments((content[i]));
			split(line, content[i], " \t");
			for (size_t j = 0; j < line.size(); j++)
				content_parsed += line[j] + " ";
			line.clear();
			treated_line.clear();
			content_parsed += "\n";
		}
		loc->parse(d, content_parsed);
		d->addLocation(loc);
	}
	catch (exception &e)
	{
		delete loc;
		throw logic_error(e.what());
	}
	return (last - 1);
}

void	ParseDirectives::save_root( Directives *d, const StrVector &line ) {
	if ( line.size() != 2 )
		throw logic_error("Unexpected amount of arguments");
	if (d->dirSet["alias"] == true)
		throw logic_error("Alias already setted before");
	d->root.clear();
	d->root = line[1].substr(0, line[1].find_first_of(";"));
}

void	ParseDirectives::save_listen( Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_last_of(";"));
	size_t pos = value.find_last_of(":");
	if (pos != string::npos) {
		d->ip = ParseContent::decompressIp(value.substr(0, pos));
		ParseContent::checkValidIp(d->ip);
		d->port = stoui(value.substr(pos + 1, value.length()));
	}
	else
		d->port = stoui(value);
	ParseContent::checkValidPort(d->port);
	
}

void	ParseDirectives::save_error_page( Directives *d, const StrVector & line ) {
	size_t size = line.size();
	string value;
	if (size < 3)
		throw logic_error("Unexpected amount of arguments");
	value = line[size - 1].substr(0, line[size - 1].find_first_of(";"));
	for (size_t i = 1; i < size - 1; i++)
	{
		unsigned int key = stoui(line[i]);
		if (key < 300 || key > 599)
			throw logic_error("Value " + line[i] + " not valid for error_page");
		if (d->errorPageSet(key))
			d->error_page.erase(key);
		d->error_page.insert(UintStrPair(key, value));
	}
	
}

void	ParseDirectives::save_server_name( Directives *d, StrVector & line ) {
	if (line.size() < 2)
		throw logic_error("Unexpected amount of arguments");
	if (!line[line.size() - 1].compare(";"))
		line.pop_back();
	else
		line[line.size() - 1].pop_back();
	for (size_t i = 1; i < line.size(); i++)
		d->server_names.push_back(line[i]);
}

void	ParseDirectives::save_client_max_body_size( Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	
	size_t pos_first_letter = value.find_first_not_of(ISNUM);
	unsigned int num = stoui(value.substr(0, pos_first_letter));
	string letter = value.substr(pos_first_letter, value.length());

	if (letter.empty())
	{
		if (num > 2097152)
			throw logic_error("Value " + value + " invalid, the value is limmited to 2000000k");
		d->client_max_body_size = num;
	}
	if (letter.length() > 1 || letter.find_first_of("kmgKMG") == string::npos)
		throw logic_error("Value " + value + " invalid.");
	if (value[pos_first_letter] == 'k' || value[pos_first_letter] == 'K')
	{
		if (num > 2097152)
			throw logic_error("Value " + value + " invalid, the value is limmited to 2000000k");
		d->client_max_body_size = num;
	}
	else if (value[pos_first_letter] == 'm' || value[pos_first_letter] == 'M')
	{
		if (num > 2048)
			throw logic_error("Value " + value + " invalid, the value is limmited to 2000m");
		d->client_max_body_size = num * 1024;
	}
	else if (value[pos_first_letter] == 'g' || value[pos_first_letter] == 'G')
	{
		if (num > 2)
			throw logic_error("Value " + value + " invalid, the value is limmited to 2g");
		d->client_max_body_size = num * 1024 * 1024;
	}
}

void	ParseDirectives::save_upload_store( Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	d->upload_store = value;
}

void	ParseDirectives::save_index(Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	d->index = value;
}

void	ParseDirectives::save_autoindex(Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	if (!value.compare("on"))
		d->autoindex = true;
	else if (!value.compare("off"))
		d->autoindex = false;
	else
		throw logic_error("Value " + value + " not valid for autoindex");
}

void	ParseDirectives::save_alias( Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	if (d->dirSet["root"] == true)
		throw logic_error("Root already setted before");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	d->alias = value;
}

void	ParseDirectives::save_allow_upload( Directives *d, const StrVector & line ) {
	if (line.size() != 2)
		throw logic_error("Unexpected amount of arguments");
	string value = line[1].substr(0, line[1].find_first_of(";"));
	if (!value.compare("on"))
		d->allow_upload = true;
	else if (!value.compare("off"))
		d->allow_upload = false;
	else
		throw logic_error("Value " + value + " not valid for allow_upload");
}

void	ParseDirectives::save_return( Directives *d, const StrVector & line ) {
	string value;
	int code = HTTP_TEMP_RED_CODE;
	string	uri;
	
	if (line.size() < 2 || line.size() > 3)
		throw logic_error("Unexpected amount of arguments");
	if (line.size() == 2)
	{
		value = line[1].substr(0, line[1].find_first_of(";"));
		if (ParseContent::checkHttpPrefix(value))
			uri = value;
		else
			code = -1;
	}
	else{
		value = line[2].substr(0, line[2].find_first_of(";"));
		code = ParseContent::checkErrorCode(line[1]);
		if (ParseContent::checkHttpPrefix(value))
			uri = value;
		else
			code = -1;
	}
	d->_return = IntStrPair(code, value);
}

void	ParseDirectives::save_allow_methods( Directives *d, const StrVector & line ) {
	string	value;
	
	if (line.size() > 4)
		throw logic_error("Unexpected amount of arguments");
	for (size_t i = 1; i < line.size(); i++) {
		if (i != line.size() - 1 && line[i].find_first_of(";") != string::npos)
			throw logic_error("Unexpected token ; in value " + line[i]);
		else if (i == line.size() - 1)
			value = line[i].substr(0, line[i].find_first_of(";"));
		else
			value = line[i];
		if (value.compare("POST") && value.compare("GET") && value.compare("DELETE"))
			throw logic_error("Value \"" + value + "\" not valid for allow_methods");
		d->allow_methods.push_back(value);
	}
}

void	ParseDirectives::save_cgi( Directives *d, const StrVector & line ) {
	string ext;
	string path;
	
	if (line.size() != 3)
		throw logic_error("Unexpected amount of arguments");
	
	ext = line[1];
	path = line[2].substr(0, line[2].find_first_of(";"));
	if (ext[0] != '.')
		throw logic_error("Invalid extension \"" + ext + "\" in cgi");
	if (d->cgiSet(ext))
		d->cgi.erase(ext);
	d->cgi.insert(StrPair(ext, path));
}

void	ParseDirectives::checkDuplicate( const string & directive, StrBoolMap dirSet) {
	if ( dirSet[ directive ] == true && ParseContent::canRepeatDirectiveList.at( directive ) == false )
		throw logic_error( "\"" + directive + "\" directive is duplicate" );
}

int	ParseDirectives::parseServerDirectives( Directives *d, StrVector & line, int type, StrVector &content, int n_line ) {
	ParseDirectives::checkDuplicate(line[0], d->dirSet);
	d->dirSet[ line[0] ] = true;
	switch (type)
	{
		case 0:
			ParseDirectives::save_root(d, line);
			break ;
		case 1:
			ParseDirectives::save_listen(d, line);
			break ;
		case 2:
			ParseDirectives::save_error_page(d, line);
			break ;
		case 3:
			ParseDirectives::save_server_name(d, line);
			break ;
		case 4:
			ParseDirectives::save_client_max_body_size(d, line);
			break ;
		case 5:
			ParseDirectives::save_upload_store(d, line);
			break ;
		case 6:
			ParseDirectives::save_autoindex(d, line);
			break ;
		case 7:
			n_line = ParseDirectives::parseLocation(d, content, n_line);
			break ;
		default:
			throw logic_error("\"" + line[0] + "\" not a valid directive for Server");
	}
	return (n_line);
}

void	ParseDirectives::parseLocationDirectives( Directives *d, StrVector & line, int type ) {
	ParseDirectives::checkDuplicate(line[0], d->dirSet);
	d->dirSet[ line[0] ] = true;
	switch (type)
	{
		case 0:
			ParseDirectives::save_root(d, line);
			break ;
		case 1:
			ParseDirectives::save_error_page(d, line);
			break ;
		case 2:
			ParseDirectives::save_upload_store(d, line);
			break ;
		case 3:
			ParseDirectives::save_allow_upload(d, line);
		case 4:
			ParseDirectives::save_index(d, line);
			break ;
		case 5:
			ParseDirectives::save_alias(d, line);
			break ;
		case 6:
			ParseDirectives::save_autoindex(d, line);
			break ;
		case 7:
			ParseDirectives::save_return(d, line);
			break ;
		case 8:
			ParseDirectives::save_allow_methods(d, line);
			break ;
		case 9:
			ParseDirectives::save_cgi(d, line);
			break ;
		default:
			throw logic_error("\"" + line[0] + "\" not a valid directive for Location");
	}
}