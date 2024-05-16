/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logs.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 09:40:43 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 12:11:41 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Logs.hpp>

Logs::Logs( void ) {
	return ;
}

Logs::Logs( string logFileN ) {
	this->_fileName = logFileN;
	this->_logDirectory = "./logs/";
	this->_logFile.open(this->_logDirectory + this->_fileName, ofstream::out);
	if (this->_logFile.is_open() == false)
	{
		cerr << "Error creating logfile " << logFileN << endl;
		return ;
	}
	this->Debug("Created " + this->_fileName + " successfully");
	return ;
}

Logs	&Logs::operator=( const Logs &src ) {
	if (this != &src)
		this->_logFile.open(src._logDirectory + src._fileName, ofstream::out);
	return (*this);
}

Logs::~Logs( void ) {
	return ;
}

void	Logs::Info( string msg ) {
	this->_logFile << "[INFO]	" << msg << endl;
}

void	Logs::Warning( string msg ) {
	this->_logFile << "[WARNING]	" << msg << endl;
}

void	Logs::Error( string msg ) {
	this->_logFile << "[ERROR]	" << msg << endl;
}

void	Logs::Debug( string msg ) {
	this->_logFile << "[DEBUG]	" << msg << endl;
}