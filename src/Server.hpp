/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mki <mki@student.42seoul.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 10:52:07 by mki               #+#    #+#             */
/*   Updated: 2021/09/06 11:03:07 by mki              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

// std::cin
// std::cout
#include <iostream>

// std::string
#include <string>

// atoi
#include <stdlib.h>

// push_back
#include <vector>

class Server
{
private:
public:
    // canonical form
    Server();
    Server(int port, std::string dir);
    Server(const Server &server);
    const Server &operator=(const Server &server);
    ~Server();

    int init_server();
    void run();
};

#endif
