/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.ipp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:17:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/29 15:43:26 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEDIRECTIVES_IPP__
# define __PARSEDIRECTIVES_IPP__

# include <ParseContent.hpp>

template <typename T>
void	ParseContent::save_root(std::string head, T &config) {
	std::cout << "Hola que ttal??" << std::endl;
	std::cout << head << std::endl;
	(void)config;
};

#endif