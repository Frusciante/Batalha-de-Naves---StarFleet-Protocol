#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "util_server.h"

int main(int argc, char* argv[])
{
    int serv_sock = 0;
    int ip_type;
    int port;
    socklen_t clnt_addr_len;
    struct sockaddr_storage clnt_addr;
    char error_string[ERR_STRING_LEN] = {};
    int clnt_sock;

    setlocale(LC_ALL, "");

    if (argc != 3)
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "Usage : %s <IP type (v4 or v6)> <port>\n", argv[0]);
        error_handling(error_string, 0, 1);
    }

    if (strcmp(argv[1], "v4") == 0) { ip_type = AF_INET; }
    else if (strcmp(argv[1], "v6") == 0) { ip_type = AF_INET6; }
    else
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "IP type should be 'v4' or 'v6'. Your input : %s", argv[1]);
        error_handling(error_string, 0, 1);
    }

    port = atoi(argv[2]);
    if (port <= 1023 || port > 65535)
    {
        snprintf(error_string, ERR_STRING_LEN - 1, "Port should be an integer between 1024 and 65535. Your input : %s", argv[2]);
        error_handling(error_string, 0, 1);
    }

    serv_sock = init_socket(ip_type, (unsigned short)port);
    switch (serv_sock)
    {
    case -1:
        error_handling("NULL pointer error occured.", 0, 1);
    case 1:
        error_handling("Error occured while initizlizing a socket", 1, 1);
    }

    while (1)
    {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
        {
            error_handling("accpet() error occured", 1, 0);
            continue;
        }
        
        srand(time(NULL));
        if (-1 == battle(clnt_sock)) { error_handling("The game closed unexpectedly.", 0, 0); }
        // sleep(1);
        close(clnt_sock);
    }

    return 0;
}
