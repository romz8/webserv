/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 14:30:01 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 10:27:04 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEDIRECTIVES_HPP__
# define __PARSEDIRECTIVES_HPP__

# include <Utils.hpp>
# include <TypeDefines.hpp>
# include <ParseContent.hpp>
# include <Directives.hpp>
# include <cstdlib>

class Directives;

class	ParseDirectives {
	public:
		ParseDirectives( void );
		~ParseDirectives( void );

		
		static int	parseServer( Directives *, StrVector &, int );
		static int	parseLocation( Directives *, StrVector &, int );
		static int	parseServerDirectives( Directives *, StrVector &, int , StrVector &, int );
		static void	parseLocationDirectives( Directives *, StrVector &, int );
		static void	save_root( Directives *, const StrVector & );
		static void	save_listen( Directives *, const StrVector & );
		static void	save_error_page( Directives *, const StrVector & );
		static void	save_server_name( Directives *, StrVector & );
		static void	save_client_max_body_size( Directives *, const StrVector & );
		static void	save_upload_store( Directives *, const StrVector & );
		static void	save_index( Directives *, const StrVector & );
		static void	save_autoindex( Directives *, const StrVector & );
		static void	save_alias( Directives *, const StrVector & );
		static void	save_allow_upload( Directives *, const StrVector & );
		static void save_return( Directives *, const StrVector & );
		static void	save_allow_methods( Directives *, const StrVector & );
		static void	save_cgi( Directives *, const StrVector &);
		static void	checkDuplicate( const string & directive, StrBoolMap dirSet);
};

#endif