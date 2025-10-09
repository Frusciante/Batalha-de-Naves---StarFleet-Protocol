#include "util_server.h"

int init_socket(int address_family, unsigned short target_port)
{
    int sock = socket(address_family, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr_v4 = {};
    struct sockaddr_in6 serv_addr_v6 = {};
    struct sockaddr* serv_addr = NULL; 

    if (-1 == sock)
    {
        return ERR_SOCKET;
    }

    switch (address_family)
    {
    case AF_INET:
        serv_addr_v4.sin_family = AF_INET;
        serv_addr_v4.sin_port = htons(target_port);
        serv_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr = (struct sockaddr*)&serv_addr_v4;
        break;
    
    case AF_INET6:
        serv_addr_v6.sin6_family = AF_INET6;
        serv_addr_v6.sin6_addr = in6addr_any;
        serv_addr_v6.sin6_port = htons(target_port);
        serv_addr = (struct sockaddr*)&serv_addr_v6;
        break;
    }
    
    if (!serv_addr)
    {
        return -1;
    }

    if (-1 == bind(sock, serv_addr, (address_family == AF_INET ? sizeof(serv_addr_v4) : sizeof(serv_addr_v6))))
    {
        return ERR_BIND;
    }

    if (-1 == listen(sock, BACKLOG_SIZE))
    {
        return ERR_LISTEN;
    }
    
    return sock;
}

int get_random_opt(void)
{
    return rand() % ACTION_CNT;
}