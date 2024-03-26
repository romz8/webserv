/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_socket.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 16:05:34 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/26 16:37:17 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h> // For read
#include <exception>
#include <iostream>
#include <string>

#define PORT 9999

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;
	struct sockaddr_in client_addr;
	size_t addr_size = sizeof(sockaddr_in);

	//creating the socket for the client, on ipv4 (AF_INET) and TCP (SOCK_STREAM)
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	
	client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
	
	int connection = connect(client_socket, (struct sockaddr *) &client_addr, addr_size);
	if (connection < 0)
	{
		std::cerr << "Error connecting to server" << std::endl;
		return 1;
	}
	std::string message = argv[1];
	send(client_socket, message.c_str(), message.size(), 0);
	std::cout << "[client browser] Message sent" << std::endl;

	char buffer[1024];
	size_t valread = read(client_socket, &buffer, 1024);
	std::cout << "[client browser] Message received: " << buffer << std::endl;
	close(client_socket);
	return 0;
}