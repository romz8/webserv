/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directives.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:14 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/11 16:02:41 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Directives.hpp>

Directives::Directives( void ) : root("/"), client_max_body_size(10240), allow_upload(false), autoindex(false) {
	this->logs = Logs("directives_logs");
	this->port = 443;
	this->ip = "0.0.0.0";
	this->def_err_page = "./def_err.html";
	this->upload_store = "./uploads/";
	for (int i = 0; i < N_DIRECTIVES; i++)
		this->dirSet[ParseContent::total_directives[i]] = false;
	return ;
}

Directives::~Directives( void ) {
	this->cleanServers();
	this->cleanLocations();
	return ;
}

Directives::Directives( Directives & src ) {
	*this = src;
}

Directives::Directives( Directives & src, int ) {
	*this = src;
	this->port = src.getPort();
	this->client_max_body_size = src.client_max_body_size;
	this->server_names = src.getSNames();
	this->ip = src.getIp();
	this->dupLocations(src.locations);
}

Directives	&Directives::operator=( Directives & src ) {
	if (this != &src)
	{
		this->logs = src.logs;
		this->root = src.getRoot();
		this->error_page = src.getErrorPages();
		this->upload_store = src.getUploadStore();
		this->autoindex = src.getAutoindex();
		this->def_err_page = src.def_err_page;
	}
	return (*this);
}

Directives	*Directives::parseDirectives( const string & content ) {
	Directives *d = new Directives;
	int		n_server = 0;
	try {
		StrVector lines;
		StrVector line;
		size_t	i = 0;
	
		split(lines, content, "\n");
		size_t n_lines = lines.size();
		while (i < n_lines)
		{
			string treated_line = treat_comments(lines[i]);
			split(line, treated_line, " \t");
			if (!line[0].compare("server") || !line[0].compare("server{"))
			{
				n_server++;
				i = ParseDirectives::parseServer(d, lines, i);
			}
			if (line[0].compare("server"))
				throw logic_error("Unexpected directive out of server");
			line.clear();
			treated_line.clear();
		}
		return (d);
	}
	catch ( exception &e ) {
		string error = "In server " + to_string(n_server) + ": " + e.what();
		d->logs.Error(error);
		delete d;
		throw logic_error(error);
	}
	return (NULL);
}

void								Directives::setServersToListen( void ) {
	for (size_t i = 0; i < this->servers.size(); i++) {
		this->servers[i]->setlisten();
	}
}

const string						&Directives::getIp( void ) const {
	return (this->ip);
}

const unsigned int					&Directives::getPort( void ) const {
	return (this->port);
}

const string						&Directives::getRoot( void ) const {
	return (this->root);
}

const StrVector						&Directives::getSNames( void ) const {
	return (this->server_names);
}

Server								*Directives::getServer( unsigned int s_num ) const {
	if (s_num >= this->servers.size())
		return (NULL);
	return (new Server(*this->servers[s_num]));
}

Location							*Directives::getLocation( unsigned int s_num ) const {
	if (s_num >= this->locations.size())
		return (NULL);
	return (new Location(*this->locations[s_num]));
}

ServersVector						&Directives::getServers( void ) {
	return (this->servers);
}

const bool							&Directives::getAutoindex( void ) const {
	return (this->autoindex);
}

const string						&Directives::getUploadStore( void ) const {
	return (this->upload_store);
}

UintStrMap::mapped_type		&Directives::getErrorPage( unsigned int key ) {
	UintStrMap::const_iterator  It = this->error_page.find(key);
	if (It == this->error_page.end())
		return (this->def_err_page);
	return (this->error_page.at(key));
}

UintStrMap					&Directives::getErrorPages( void ) {
	return (this->error_page);
}

bool								Directives::errorPageSet( unsigned int key ) const {
	UintStrMap::const_iterator  It = this->error_page.find(key);
	if (It == this->error_page.end())
		return (false);
	return (true);
}

bool								Directives::cgiSet( string key ) const {
	StrMap::const_iterator  It = this->cgi.find(key);
	if (It == this->cgi.end())
		return (false);
	return (true);
}

const StrMap						&Directives::getCgiMap( void ) const {
	return (this->cgi);
}

const string						Directives::getCgiExe( string ext ) const {
	StrMap::const_iterator	It = this->cgi.find(ext);
	if (It == this->cgi.end())
		return ("");
	return (this->cgi.at(ext));
}

const int							&Directives::getReturnCode( void ) const {
	return (this->_return.first);
}

const string						&Directives::getReturnValue( void ) const {
	return (this->_return.second);
}

void								Directives::addServer( Server *serv ) {
	this->servers.push_back(serv);
}

void								Directives::addLocation( Location *loc ) {
	this->locations.push_back(loc);
}

void			Directives::dupLocations( const LocVector & src ) {
	for (LocVector::const_iterator it = src.begin(); it != src.end(); it++) {
		if (*it != NULL)
			this->locations.push_back( new Location( **it ));
	}
}

void			Directives::cleanLocations( void ) {
	for (LocVector::const_iterator it = this->locations.begin(); it != this->locations.end(); it++)
	{
		if (*it != NULL)
			delete *it;
	}
	this->locations.clear();
	this->dirSet["location"] = false;
}

void			Directives::cleanServers( void ) {
	ServersVector::const_iterator it;
	it = this->servers.begin();
	for (; it != this->servers.end(); it++)
	{
		if (*it != NULL)
			delete *it;
	}
	this->servers.clear();
}
