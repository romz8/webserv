/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketserver.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:57:40 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/26 16:35:16 by rjobert          ###   ########.fr       */
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
	int server_socket;
	// int client_socket;
	
	//creating the listenign socket for the server, on ipv4 (AF_INET) and TCP (SOCK_STREAM)
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	
	//define our endpoint adress and port 
	sockaddr_in server_addr;
	size_t addr_size = sizeof(sockaddr_in);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	// bind the socket to our specified IP and port
	if (bind(server_socket, (const struct sockaddr *) &server_addr, addr_size) < 0)
	{
		std::cerr << "Error binding socket" << std::endl;
		return 1;
	}

	//dedicating the socket to listening for incoming connections
	if (listen(server_socket, 5) < 0)
	{
		std::cerr << "Error listening on socket" << std::endl;
		return 1;
	}
	while (1)
	{
		//accepting the connection from the client
		sockaddr_in client_addr;
		int io_socket = accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *) &addr_size);
		if (io_socket < 0)
		{
			std::cerr << "Error accepting connection" << std::endl;
			return 1;
		}
		
		//still cannot use string as we need c sys call read() char buffer
		// c_str() pass over const char *, not good for buffer - but later on vector of char
		char buffer[1024];
		int bytesRead = read(io_socket, &buffer, 1024);
		if (bytesRead < 0)
		{
			std::cerr << "Error reading from socket" << std::endl;
			return 1;
		}
		std::cout << "[server side] Received : " << buffer << std::endl;
		std::string response = "Hello from server";
		send(io_socket, response.c_str(), response.size(), 0);
		close(io_socket);
	}
	
	close(server_socket);
	return 0;
}
