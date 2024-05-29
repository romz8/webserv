/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.ipp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:17:13 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/29 22:08:57 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEDIRECTIVES_IPP__
# define __PARSEDIRECTIVES_IPP__

# include <ParseContent.hpp>
# include <Utils.hpp>

template <typename T>
void	ParseContent::save_root(std::string head, T &config) {
	StrVector	data;
	SUtils::split(data, head, ISSPACE);
	if (data.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + data[0]);
	config.setRootDir(data[1]);
};

template <typename T>
void	ParseContent::save_listen(std::string head, T &config) {
	StrVector line;
	SUtils::split(line, head, ISSPACE);
	if (line.size() != 2)
		throw std::logic_error("Invalid number of arguments for " + line[0]);
	std::string data = line[1];
	size_t pos = data.find_first_of(":");
	int port;
	if (pos == std::string::npos)
	{
		port = std::ft_stoi(data);
		if (port < 0 || port > USHRT_MAX)
			throw std::logic_error("Invalid port " + data);
		config.setPort(std::atoi(data.c_str()));
		return ;
	}
	std::string ip = data.substr(0, pos);
	std::string s_port = data.substr(pos, std::string::npos);
	if (s_port.empty())
		throw std::logic_error("Should give a port for " + ip);
	port = sd::ft_stoi(s_port);
	if (port < 0 || port > USHRT_MAX)
			throw std::logic_error("Invalid port " + s_port);
	checkValidIp(ip);
}

#endif