/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseInit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 11:21:58 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/28 19:57:48 by jsebasti         ###   ########.fr       */
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
	// &ParseContent::listen,
	// &ParseContent::server_name,
	// &ParseContent::client_max_body_size,
	// &ParseContent::autoindex,
	// &ParseContent::error_page,
	// &ParseContent::upload_store,
	// &ParseContent::cgi;
};