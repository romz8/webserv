/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 17:13:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/08 22:05:08 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"
# include <sys/wait.h>

class Request;

class	CGI 
{
private:
	std::map<std::string, std::string>	_env;
	std::string							_path;
	std::string							_body;
	std::string							_respbody;
	std::string							_execPath;
	int		_fdin[2];
	int		_fdout[2];
	int		_status;

public:
	CGI(Request &req, std::string execpath);
	CGI(CGI const &rhs);
	~CGI(void);

	CGI		&operator=(CGI const &rhs);
	void	executeCGI(void);
	char **setEnvExecve(void);
	int getStatus(void);
	std::string getBody(void);
	void	checkCGI();

};

bool isExecutable(const std::string& path);

#endif