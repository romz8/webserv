/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseContent.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:07:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 07:38:14 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSECONTENT_HPP__
# define __PARSECONTENT_HPP__

# include <Server.hpp>
# include <DirectivesDefine.hpp>
# include <Utils.hpp>
# include <iostream>

using namespace std;

class ParseContent {
	private:
		ParseContent( void );
	
	public:
		~ParseContent( void );
		
		static int	getServAllowDirectives( const std::string & );
		static int	getLocAllowDirectives( const std::string & );

		static string				server_directives[N_SERVER_DIRECTIVES];
		static string				location_directives[N_LOCATION_DIRECTIVES];
		static string				total_directives[N_DIRECTIVES];
		static StrBoolPair			_canRepeatDirectiveList[N_DIRECTIVES];
		static StrBoolMap			canRepeatDirectiveList;
};

#endif