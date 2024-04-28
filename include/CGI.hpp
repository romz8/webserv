/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/28 18:38:36 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"

class	CGI 
{
private:
	std::map<std::string, std::string>	_env;
	std::string							_path;
	std::string							_body;
	int		_pid;
	int		_fd[2];

public:
	CGI(Request &req);
	CGI(CGI const &rhs);
	~CGI(void);

	CGI		&operator=(CGI const &rhs);
	void	executeCGI(void);
	char **setEnvExecve(void);
};

#endif