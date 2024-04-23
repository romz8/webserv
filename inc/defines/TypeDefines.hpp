/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TypeDefines.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 19:01:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/22 23:36:42 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __TYPEDEFINES_HPP__
# define __TYPEDEFINES_HPP__

# include <vector>
# include <string>
# include <map>

using namespace std;

class Server;
class Directives;
class Location;

typedef	vector< Server * >			ServersVector;
typedef	vector< string >			StrVector;
typedef vector< Location * >		LocVector;
typedef map<unsigned int, string>	UintStrMap;
typedef	map<string, bool>			StrBoolMap;
typedef map<const string, bool>		const_StrBoolMap;
typedef pair<string, bool>			StrBoolPair;
typedef pair<const string, bool>	const_StrBoolPair;
#endif