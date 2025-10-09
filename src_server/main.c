#include <stdio.h>
#include <string.h>
#include "util_server.h"

int main(int argc, char* argv[])
{
    Entity inimiga = {100, -1};
    int serv_sock = 0;
    int ip_type;
    int port;

    if (argc != 3)
    {
        printf("Usage : %s <IP type (v4 or v6)> <port>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "v4") == 0)
    {
        ip_type = IPV4;
    }
    else if (strcmp(argv[1], "v6") == 0)
    {
        ip_type = IPV6;
    }
    else
    {
        printf("IP type should be v4 or v6. Your input : %s", argv[1]);
        return 1;
    }

    port = atoi(argv[2]);
    if (port <= 1023 || port > 65535)
    {
        printf("Port should be an integer, between 1024 to 65535. Your input : %s", argv[2]);
        return 1;
    }
    
    switch (ip_type)
    {
        case IPV4:
            serv_sock = init_socket_ipv4((unsigned short)port);
            break;
        case IPV6:
            serv_sock = init_socket_ipv6((unsigned short)port);
            break;
    }

    const char* err_msg = NULL;
    switch (serv_sock)
    {
    case ERR_SOCKET:
        err_msg = "socket() error";
        break;
    case ERR_BIND:
        err_msg = "bind() error";
        break;
    case ERR_LISTEN:
        err_msg = "listen() error";
        break;
    }

    if (err_msg)
    {
        printf("%s occured. \n", err_msg);
        return 1;
    }

    srand(time(NULL));

    socklen_t clnt_addr_len;
    struct sockaddr_storage clnt_addr;
    while (1)
    {
        int clnt_sock = accept(serv_sock, &clnt_addr, &clnt_addr_len);
        if (clnt_sock = -1)
        {
            puts("accpet() error occured.");
            continue;
        }

        while (inimiga.hp > 0)
        {
            inimiga.action = get_random_opt();
            switch (inimiga.action)
            {
            case LASER_ATTACK:
                break;
            case PHOTON_TORPEDO:
                break;
            case SHIELDS_UP:
                break;
            case CLOACKING:
                break;
            case HYPER_JUMP:
            }
        }
    }

    return 0;
}
