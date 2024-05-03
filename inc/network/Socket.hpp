/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:27:44 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/03 09:27:51 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SOCKET_HPP__
# define __SOCKET_HPP__

# include <sys/socket.h>

class Socket
{
public:
    Socket( void );
    ~Socket( void );
    static socket_t createSocket( int, int, int );
};

#endif