/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 14:29:06 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/25 18:50:30 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Utils.hpp>
#include <iostream>

using namespace std;

namespace SUtils {

	// int	compareNumbersAsStrings( const string _num1, const string _num2 ) {
	// 	string num1( deleteExtraZeros( _num1 ) );
	// 	string num2( deleteExtraZeros( _num2 ) );
	// 	if (_num1.length() > _num2.length())
	// 		return (1);
	// 	if (_num1.length() < _num2.length())
	// 		return (-1);
	// 	return (_num1.compare(_num2));
	// }

	// string	deleteExtraZeros( const string number ) {
	// 	string res(number);

	// 	size_t idx = res.find_first_not_of("0");
	// 	if (idx != string::npos)
	// 		res.erase(0, idx);
	// 	else
	// 		res = "0";
	// 	return (res);
	// }

	string		&treat_comments( string & str ) {
		size_t pos = str.find_first_of("#");
	
		if ( pos != string::npos)
		{
			str = str.substr(0, pos);
			return (str);
		}
		return (str);
	}
	
	// void	parse_line( const string & line, int n_line ) {
	// 	static string prev_directive = "";
	// 	StrVector directive_line;
	// 	string	directive;
	// 	size_t separator_pos = line.find_first_of(";{}");
	
	// 	split(directive_line, line, " \t");
	// 	if (directive_line.size() == 0)
	// 		return ;
	// 	directive = directive_line[0];
	// 	if (separator_pos == string::npos && directive.compare("location") && directive.compare("server"))
	// 	{
	// 		if (directive.compare("{") && directive.compare("}"))
	// 			throw logic_error("Expected separator \";\" in line " + to_string(n_line));
	// 	}
	// 	else if (separator_pos == string::npos && (!directive.compare("location") || !directive.compare("server")))
	// 	{
	// 		prev_directive.clear();
	// 		prev_directive = directive;
	// 		return ;
	// 	}
	// 	else if (separator_pos != string::npos && directive.compare("location") && directive.compare("server"))
	// 	{
	// 		if (!directive.compare("{") && prev_directive.compare("location") && prev_directive.compare("server"))
	// 			throw logic_error("Unxpected separator \"{\" in line " + to_string(n_line));
	// 		else if (!directive.compare("{")) {
	// 			prev_directive.clear();
	// 			prev_directive = directive;
	// 		}
	// 		else if (line[separator_pos] != ';' && directive.compare("}"))
	// 			throw logic_error("Expected separator \";\" in line " + to_string(n_line));
	// 	}
	// 	else if (separator_pos != string::npos && (!directive.compare("server") || !directive.compare("location")) )
	// 	{
	// 		if ( line[separator_pos] != '{' )
	// 			throw logic_error("Expected separator \"{\" in line " + to_string(n_line));
	// 	}
	// 	else if (separator_pos == string::npos && (!prev_directive.compare("location") || !prev_directive.compare("server")))
	// 		throw logic_error("Expected separator \"{\" in line " + to_string(n_line));
	// 	prev_directive.clear();
	// 	prev_directive = directive;
	// }

	// unsigned int	stoui( const string &num )
	// {
	// 	unsigned int ret = 0;
	
	// 	if (num.find_first_not_of(ISNUM) != string::npos)
	// 		throw logic_error("Cannot do the conversion");
		
	// 	if (num.length() > 11 && num.compare("4294967295") > 0)
	// 		throw logic_error("The number is greater than UINT_MAX");
		
	// 	for (size_t i = 0; i < num.length(); i++) {
	// 		ret = (ret * 10) + (num[i]- 48);
	// 	}
		
	// 	return ret;
	// }
	
	// int	check_brackets( StrVector & content, int n_line ) {
	// 	StrVector line;
		
	// 	string treated_line = SUtils::treat_comments(content[n_line]);
	// 	split(line, treated_line, " \t");
	// 	treated_line.clear();
	// 	int expected_close = 1;
	// 	int expected_open = 1;
	// 	if (!line[0].compare("location"))
	// 	{
	// 		expected_close = 0;
	// 		expected_open = 0;
	// 	}
	// 	line.clear();
	// 	int brackets = 0;
	// 	int len = content.size();
	// 	for (int i = n_line; i < len; i++)
	// 	{
	// 		treated_line = SUtils::treat_comments(content[i]);
	// 		split(line, treated_line, " \t");
	
	// 		size_t size = line.size();
	// 		for (size_t j = 0; j < size; j++)
	// 		{
	// 			if (!line[j].compare("location"))
	// 			{
	// 				expected_open++;
	// 				expected_close++;
	// 			}
	// 			else if (line[j].find_first_of('{') != string::npos && expected_open > 0)
	// 			{
	// 				brackets++;
	// 				expected_open--;
	// 			}
	// 			else if (line[j].find_first_of('}') != string::npos && expected_open == 0)
	// 			{
	// 				brackets--;
	// 				expected_close--;
	// 			}
	// 		}
	// 		if (brackets == 0 && expected_close == 0 && expected_open == 0)
	// 			return (i + 1);
	// 		line.clear();
	// 	}
	// 	if (expected_close > 0)
	// 		throw logic_error("Something not closed");
	// 	else if (expected_open > 0)
	// 		throw logic_error("Something not opened");
	// 	return (ERROR);
	// }
	
	// int		SUtils::check_brackets( const StrVector & content, int n_line ) {
		
	// }
	
	
	StrVector &	split( StrVector & v, string strArr, string delimiter ) {
		string	temp;
		size_t		starting;
		size_t		ending;
		starting = 0;
		ending = strArr.find_first_of( delimiter );
		while ( ending != string::npos )
		{
			temp = strArr.substr( starting, ending - starting );
			if ( temp.length() > 0 )
				v.push_back( temp );
			starting = ending + 1;
			ending = strArr.find_first_of( delimiter, starting );
		}
		temp = strArr.substr( starting, ending - starting );
		if ( temp.length() > 0 )
			v.push_back( temp );
		return ( v );
	}

	string	trim( string str ) {
		return (trim_right( trim_left( str ) ) );
	}

	int		get_pair( string &content, string &head, string &body ) {
		size_t	pos;
		size_t	bracet;

		content = trim_left( content );
		pos = content.find_first_of("{;");
		if (pos == std::string::npos)
			return (NOT_SEPARATOR);
		if (content[ pos ] == ';')
		{
			split_pair(content, head, body);
			content.erase(0, pos + 1);
			content = trim_left(content);
			return (SEMICOLON);
		}
		bracet = SUtils::checkBracets(content);
		if (bracet == std::string::npos)
			throw std::logic_error("Expected }");
		head = SUtils::trim( content.substr(0, pos + 1) );
		body = SUtils::trim( content.substr(pos + 1, bracet - pos - 1) );
		content.erase(0, bracet + 1);
		content = trim_left(content);
		return (BRACET);
	}

	size_t	checkBracets( string content ) {
		int			level;
		size_t		bracet;
		size_t		ret;
		std::string	copy;

		level = 0;
		ret = 0;
		copy = content;
		do
		{
			bracet = content.find_first_of( "{}" );
			if ( bracet == std::string::npos \
					|| ( content[ bracet ] == '}' && level == 0 ) )
				return ( std::string::npos );
			level += content[ bracet ] == '{' ? 1 : -1;
			content.erase( 0, bracet + 1 );
			ret += bracet + 1;
		} while ( level > 0 );
		ret--;
		return ( ret );
	}
}

namespace std {
	template <typename T>
	string	to_string( const T &n )
	{
		ostringstream	ss;
		ss << n;
		return (ss.str());
	}
}

void	split_pair( const string str, string &head, string &body ) {
	size_t		pos;
	std::string	aux;

	body = "";
	aux = SUtils::trim( str );
	pos = aux.find_first_of( ISSPACE );
	if ( pos != std::string::npos )
	{
		head = aux.substr( 0, pos );
		body = aux.substr( pos, std::string::npos );
		body = trim_left( body );
	}
	else
		head = aux;
}

string trim_left( string str ) {
	size_t i = 0;

	while (i < str.length() && isspace(str[i]))
		i++;
	return (str.substr(i, str.length()));
}

string trim_right( string str ) {
	int	i;

	i = str.length() - 1;
	while ( i >= 0 && isspace( str[ i ] ) )
		i--;
	return ( str.substr( 0, i + 1 ) );
}
