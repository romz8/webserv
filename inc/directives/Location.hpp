/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 21:26:21 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 19:35:08 by jsebasti         ###   ########.fr       */
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
        Directives  *_d;
        std::string _uri;
        bool        _isDir;

    public:
        Location( void );
        ~Location( void );
		Location( Location & );
		Location	&operator=( Location & );
        
        const std::string   &getUri( void ) const;

        void    setUri( const std::string & );
        void    parse( const std::string & );
        void    parse_location_line( Directives *, const std::string & );
        
		UintStrMap::mapped_type			&getErrorPage( unsigned int ) const ;
};

#endif