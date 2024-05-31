/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseInit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 11:21:58 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/31 12:15:49 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Parser.hpp>
# include <ParseContent.hpp>

void Parser::init( void ) {
	for (int i = 0; i < DIRECTIVES_NUM; i++)
		this->_directives.push_back(ParseContent::total_directives[i]);
	for (int i = 0; i < N_SIMPLE_DIRECTIVES; i++)
		this->_simpleDirectives.push_back(ParseContent::simple_directives[i]);
	for (int i = 0; i < N_COMPLEX_DIRECTIVES; i++)
		this->_complexDirectives.push_back(ParseContent::complex_directives[i]);
	this->_allowed_directives.push_back("server");
}

Parser::parseSimpleSA Parser::_parseSimpleSA = {
	&ParseContent::save_root<ServerConfig>,
	&ParseContent::save_listen,
	&ParseContent::save_server_name,
	&ParseContent::save_client_max_body_size,
	&ParseContent::save_autoindex<ServerConfig>,
	&ParseContent::save_error_page<ServerConfig>,
	&ParseContent::save_allow_methods<ServerConfig>,
};

Parser::parseSimpleLA Parser::_parseSimpleLA = {
	&ParseContent::save_root<LocationConfig>,
	&ParseContent::save_error_page<LocationConfig>,
	&ParseContent::save_autoindex<LocationConfig>,
	&ParseContent::save_upload_dir,
	&ParseContent::save_allow_upload,
	&ParseContent::save_index,
	&ParseContent::save_allow_methods<LocationConfig>,
	&ParseContent::save_return,
	&ParseContent::save_cgi
};