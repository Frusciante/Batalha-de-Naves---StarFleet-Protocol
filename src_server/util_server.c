#include "util_server.h"

int init_socket_ipv4(short target_port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return ERR_SOCKET;
    }
}

int init_socket_ipv6(short target_port)
{

}

int get_random_opt(void)
{
    return rand() % ACTION_CNT;
}