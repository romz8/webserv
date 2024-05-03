/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 21:26:21 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 07:47:01 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

# include "Directives.hpp"
# include "Defines.hpp"
# include "DirectivesDefine.hpp"

class Location
{
    private:
        Directives  *directives;
        std::string uri;
        bool        isDir;

    public:
        Location( void );
        ~Location( void );
		Location( const Location & );
		Location	&operator=( const Location & );
        
        const std::string   &getUri( void ) const;

        void    setUri( const std::string & );
        void    parse( Directives *, const std::string & );
        void    parse_location_line( const std::string & );
        
		const UintStrMap::mapped_type			&getErrorPage( unsigned int ) const ;
};

#endif