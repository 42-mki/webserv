/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mki <mki@student.42seoul.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 10:52:02 by mki               #+#    #+#             */
/*   Updated: 2021/09/06 10:59:38 by mki              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// pthread_create
// pthread_join
#include <pthread.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        Server server(8000, "./htdocs");

        if (server.init_server() == 0)
        {
            return (-1);
        }
        server.run();
    }
    return (0);
}
