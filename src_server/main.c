#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "util_server.h"

extern const int OPT_ON;
extern const int KEEP_IDLE_TIME;
extern const int KEEP_INTERVAL;
extern const int KEEP_CNT;

int main(int argc, char* argv[])
{
    char clnt_addr_buf[INET6_ADDRSTRLEN] = {};
    int battle_result = 0;
    int serv_sock = 0;
    int ip_type;
    int port;
    socklen_t clnt_addr_len;
    struct sockaddr_in clnt_addr_v4 = {};
    struct sockaddr_in6 clnt_addr_v6 = {};
    struct sockaddr* clnt_addr = 0;
    char error_string[ERR_STRING_LEN] = {};
    int clnt_sock;

    setlocale(LC_ALL, "");
    srand(time(NULL));
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "Usage : %s <IP type (v4 or v6)> <port>", argv[0]);
        error_handling(error_string, __func__, __LINE__, 0);
        return 1;
    }

    if (strcmp(argv[1], "v4") == 0) { ip_type = AF_INET; }
    else if (strcmp(argv[1], "v6") == 0) { ip_type = AF_INET6; }
    else
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "IP type should be 'v4' or 'v6'. Your input : %s", argv[1]);
        error_handling(error_string, __func__, __LINE__, 0);
        return 1;
    }

    port = atoi(argv[2]);
    if (port <= 1023 || port > 65535)
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "Port should be an integer between 1024 and 65535. Your input : %s", argv[2]);
        error_handling(error_string, __func__, __LINE__, 0);
        return 1;
    }

    serv_sock = init_socket(ip_type, (unsigned short)port);
    switch (serv_sock)
    {
    case -1:
        error_handling("NULL pointer error occured.", __func__, __LINE__, 0);
        return 1;
    case 1:
        error_handling("Error occured while initizlizing a socket", __func__, __LINE__, 0);
        return 1;
    }
    
    switch (ip_type)
    {
    case AF_INET:
        clnt_addr = (struct sockaddr *)&clnt_addr_v4;
        clnt_addr_len = sizeof(clnt_addr_v4);
        break;
    case AF_INET6:
        clnt_addr = (struct sockaddr *)&clnt_addr_v6;
        clnt_addr_len = sizeof(clnt_addr_v6);
        break;
    }

    while (1)
    {
        puts("\nAccept waiting...");
        clnt_sock = accept(serv_sock, clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
        {
            error_handling("accpet() error occured", __func__, __LINE__, 1);
            continue;
        }

        switch (clnt_addr->sa_family)
        {
        case AF_INET:
            if (NULL != inet_ntop(AF_INET, &((struct sockaddr_in*)clnt_addr)->sin_addr, clnt_addr_buf, sizeof(clnt_addr_buf)))
            {
                printf("\nConnected with user.\nIP - %s\n", clnt_addr_buf);
            }
            else
            {
                error_handling("inet_ntop() error occured", __func__, __LINE__, 1);
                close(clnt_sock);
                continue;
            }
            break;
        case AF_INET6:
            if (NULL != inet_ntop(AF_INET6, &((struct sockaddr_in6*)clnt_addr)->sin6_addr, clnt_addr_buf, sizeof(clnt_addr_buf)))
            {
                printf("\nConnected with user.\nIP - %s\n", clnt_addr_buf);
            }
            else
            {
                error_handling("inet_ntop() error occured", __func__, __LINE__, 1);
                close(clnt_sock);
                continue;
            }
            break;
        }        

        setsockopt(clnt_sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&OPT_ON, sizeof(OPT_ON));
        setsockopt(clnt_sock, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&KEEP_IDLE_TIME, sizeof(KEEP_IDLE_TIME));
        setsockopt(clnt_sock, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&KEEP_INTERVAL, sizeof(KEEP_INTERVAL));
        setsockopt(clnt_sock, IPPROTO_TCP, TCP_KEEPCNT, (void *)&KEEP_CNT, sizeof(KEEP_CNT));

        battle_result = battle(clnt_sock);

        switch (battle_result)
        {
        case 0:
            puts("The game finished successfully.");
            break;

        default:
            error_handling("The game closed unexpectedly.", __func__, __LINE__, 0);
            break;
        }

        close(clnt_sock);
        memset(clnt_addr_buf, 0, sizeof(clnt_addr_buf));
    }

    close(serv_sock);

    return 0;
}
