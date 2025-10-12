#include "util_client.h"

extern const int OPT_ON;
extern const int KEEP_IDLE_TIME;
extern const int KEEP_INTERVAL;
extern const int KEEP_CNT;

int init_socket(const char* ip_str, const char* port_str)
{
    struct sockaddr_in serv_addr_v4 = {};
    struct sockaddr_in6 serv_addr_v6 = {};
    struct sockaddr* serv_addr = NULL;
    socklen_t serv_addr_len;
    int port;
    int sock;
    int ip_type;

    if (!(ip_str && port_str))
    {
        return -2;
    }

    port = atoi(port_str);
    if (port < 1024 || port > 65535)
    {
        return -4;
    }

    if (1 == inet_pton(AF_INET, ip_str, (void *)&serv_addr_v4.sin_addr))
    {
        ip_type = serv_addr_v4.sin_family = AF_INET;
        serv_addr_v4.sin_port = htons((unsigned short)port);
        serv_addr = (struct sockaddr*)&serv_addr_v4;
        serv_addr_len = sizeof(serv_addr_v4);
    }
    else if (1 == inet_pton(AF_INET6, ip_str, (void *)&serv_addr_v6.sin6_addr))
    {
        ip_type = serv_addr_v6.sin6_family = AF_INET6;
        serv_addr_v6.sin6_port = htons((unsigned short)port);
        serv_addr = (struct sockaddr*)&serv_addr_v6;
        serv_addr_len = sizeof(serv_addr_v6);
    }
    else
    {
        return -3;
    }

    sock = socket(ip_type, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        return -1;
    }
    if (-1 == connect(sock, serv_addr, serv_addr_len))
    {
        return -1;
    }

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&OPT_ON, sizeof(OPT_ON));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&KEEP_IDLE_TIME, sizeof(KEEP_IDLE_TIME));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&KEEP_INTERVAL, sizeof(KEEP_INTERVAL));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (void *)&KEEP_CNT, sizeof(KEEP_CNT));

    return sock;
}

int battle(int sock)
{
    BattleMessage bm = {};
    char input_buffer[11];
    int input_int = -1;

    for (;;)
    {
        switch (recv(sock, &bm, sizeof(bm), 0))
        {
        case 0:
            close(sock);
            error_handling("Lost connection to the server.\n", 0, 1);
            return 2;
        case -1:
            close(sock);
            error_handling(ERR_CONNECTION, 1, 1);
            return 2;
        }
        switch (bm.type)
        {
        case MSG_ACTION_REQ:
            for (;;)
            {
                printf("%s", bm.message);
                fgets(input_buffer, sizeof(input_buffer), stdin);
                input_int = atoi(input_buffer);
                if (0 == input_int && 0 != strcmp(input_buffer, "0\n")) { printf(ERR_MSG); }
                else { break; }
            }
            bm.type = MSG_ACTION_RES;
            bm.client_action = input_int;
            if (-1 == send(sock, &bm, sizeof(bm), 0))
            {
                close(sock);
                error_handling(ERR_CONNECTION, 1, 1);
                return 2;
            } 
            break;
        case MSG_BATTLE_RESULT:
        case MSG_INIT:
        case MSG_GAME_OVER:
        case MSG_ESCAPE:
            printf("%s", bm.message);
            break;
        case MSG_INVENTORY:
            printf("\nInventário final:\n- Seu HP restante: %d\n- HP inimigo restante: %d\n- Total de turnos jogados: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n", 
                    bm.client_hp, bm.server_hp, bm.client_action, bm.client_torpedoes, bm.client_shields);
            printf("%s", bm.message);
            return 0;
        }
    }

    return 0;
}