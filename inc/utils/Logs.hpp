/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 09:26:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 12:25:29 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LOGS_HPP__
# define __LOGS_HPP__

#include <fstream>
#include <iostream>

using namespace std;

class Logs {
	private:
		string		_fileName;
		string		_logDirectory;
		ofstream	_logFile;

	public:
		Logs( void );
		Logs( string logFileN );
		~Logs( void );

		Logs	&operator=( const Logs & );
		
		void	Info( string );
		void	Warning( string );
		void	Error( string );
		void	Debug( string );
};

#endif