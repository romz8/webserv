/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 21:26:21 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/16 20:20:39 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LOCATIONCONFIG_HPP__
# define __LOCATIONCONFIG_HPP__

# include "Directives.hpp"
# include "Defines.hpp"
# include "DirectivesDefine.hpp"

class LocationConfig
{
    private:
        Directives  *_d;
        std::string _uri;
        bool        _isDir;

    public:
        LocationConfig( void );
        ~LocationConfig( void );
		LocationConfig( Location & );
		LocationConfig	&operator=( Location & );
        
        const std::string   &getUri( void ) const;

        void    setUri( const std::string & );
        void    parse( const std::string & );
        void    parse_location_line( Directives *, const std::string & );
        
		UintStrMap::mapped_type			&getErrorPage( unsigned int ) const ;
};

#endif