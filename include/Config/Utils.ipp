/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.ipp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 15:48:52 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 02:46:51 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_IPP__
# define __UTILS_IPP__

# include <Utils.hpp>

namespace SUtils {
	template <typename T>
	int	easyfind( typename T::iterator begin, typename T::iterator end, std::string toFind ) {
		typename T::iterator	it;

		it = begin; 
		while ( it != end && *it != toFind )
			it++;
		if ( it == end )
			return ( -1 );
		return ( it - begin );
	}
};

namespace std {
	template <typename T>
	string	to_string( const T &n ) {
		ostringstream ss;

		ss << n;
		return (ss.str());
	}
	
	template <typename T>
	T	stoui( string num )
	{
		T ret = 0;
	
		if (num.find_first_not_of(ISNUM) != string::npos)
			throw logic_error("Cannot do the conversion");
		
		if (COMPARE_NBR_MAX_STR(num, T) > 0)
			throw logic_error("The number is greater than SIZE_T_MAX");
		
		for (size_t i = 0; i < num.length(); i++) {
			ret = (ret * 10) + (num[i]- 48);
		}
		
		return ret;
	}
};

#endif
