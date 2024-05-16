/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TypeDefines.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 19:01:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/16 20:55:24 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __TYPEDEFINES_HPP__
# define __TYPEDEFINES_HPP__

# include <vector>
# include <string>
# include <map>

using namespace std;

class ServerConfig;
class Directives;
class Location;

typedef	vector< ServerConfig * >					ServersVector;
typedef	vector< string >					StrVector;
typedef vector< Location * >				LocVector;

typedef map<unsigned int, string>			UintStrMap;
typedef pair<unsigned int, string>			UintStrPair;

typedef	map<string, bool>					StrBoolMap;
typedef map<const string, bool>				const_StrBoolMap;
typedef pair<string, bool>					StrBoolPair;
typedef pair<const string, bool>			const_StrBoolPair;

typedef map<string, string>					StrMap;
typedef map<const string, const string>		const_StrMap;
typedef pair<string, string>				StrPair;
typedef pair<const string, const string>	const_StrPair;

typedef pair<int, string>                   IntStrPair;

#endif