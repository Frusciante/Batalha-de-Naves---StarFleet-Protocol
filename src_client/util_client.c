#include "util_client.h"

extern const int OPT_ON;
extern const int KEEP_IDLE_TIME;
extern const int KEEP_INTERVAL;
extern const int KEEP_CNT;

static const char* const STR_MSG_TEMPLATE = "Você %sServidor %sResultado: %sPlacar: Você %d x %d Inimigo\n";
static const char* const STR_MSG_LASER = "disparou um Lazer!\n";
static const char* const STR_MSG_PHOTON_TORPEDO = "disparou um Photon Torpedo!\n";
static const char* const STR_MSG_ESCUDOS = "ativou os Escudos!\n";
static const char* const STR_MSG_CLOAKING = "ativou Cloaking!\n";
static const char* const STR_MSG_HYPER_JUMP = "acionou Hyper Jump!\n";

int init_socket(const char* ip_str, const char* port_str)
{
    struct sockaddr_in serv_addr_v4 = {};
    struct sockaddr_in6 serv_addr_v6 = {};
    struct sockaddr* serv_addr = NULL;
    socklen_t serv_addr_len;
    int port;
    int sock;
    int ip_type;
    char err_buf[64];
    
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
        error_handling("inet_pton() error", __func__, __LINE__);
        return -3;
    }

    sock = socket(ip_type, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        error_handling("socket() error occured", __func__, __LINE__);
        return -1;
    }
    if (-1 == connect(sock, serv_addr, serv_addr_len))
    {
        close(sock);
        error_handling("connect() error occured", __func__, __LINE__);
        return -1;
    }

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const void *)&OPT_ON, sizeof(OPT_ON));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, (const void *)&KEEP_IDLE_TIME, sizeof(KEEP_IDLE_TIME));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (const void *)&KEEP_INTERVAL, sizeof(KEEP_INTERVAL));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (const void *)&KEEP_CNT, sizeof(KEEP_CNT));

    return sock;
}

int battle(int sock)
{
    BattleMessage bm = {};
    char input_buffer[20];
    int input_int = -1;
    const char* user_action_str = NULL;
    const char* server_action_str = NULL;
    char* str_start_ptr = NULL;
    
    for (;;)
    {
        switch (recv_reliable(sock, &bm, sizeof(bm), 0))
        {
        case sizeof(bm):
            break;
        case 0:
            error_handling("Lost connection to the server.", __func__, __LINE__);
            return 1;
        case -1:
            error_handling(ERR_CONNECTION, __func__, __LINE__);
            return 1;
        default:
            error_handling("Error occured while receiving data from the server.", __func__, __LINE__);
            return 1;
        }
        switch (bm.type)
        {
        case -1:
            puts(bm.message);
            break;
        case MSG_ACTION_REQ:
            for (;;)
            {
                printf("%s", bm.message);
                fgets(input_buffer, sizeof(input_buffer), stdin);
                str_start_ptr = get_str_start_point(input_buffer, sizeof(input_buffer));
                input_int = atoi(str_start_ptr);
                if (0 == input_int && 0 != strncmp(str_start_ptr, "0", 1)) { puts(ERR_MSG); }
                else { break; }
            }
            bm.type = MSG_ACTION_RES;
            bm.client_action = input_int;
            switch (send_reliable(sock, &bm, sizeof(bm), 0))
            {
            case sizeof(bm):
                break;
            case -1:
                error_handling(ERR_CONNECTION, __func__, __LINE__);
                return 1;
            default:
                error_handling("Error occured while sending data to server.", __func__, __LINE__);
                return 1;
            }
            
            break;
        case MSG_BATTLE_RESULT:
            switch (bm.client_action)
            {
            case LASER_ATTACK:
                user_action_str = STR_MSG_LASER;
                break;
            case PHOTON_TORPEDO:
                user_action_str = STR_MSG_PHOTON_TORPEDO;
                break;
            case SHIELDS_UP:
                user_action_str = STR_MSG_ESCUDOS;
                break;
            case CLOAKING:
                user_action_str = STR_MSG_CLOAKING;
                break;
            default:
                user_action_str = NULL;
                break;
            }
            
            switch (bm.server_action)
            {
            case LASER_ATTACK:
                server_action_str = STR_MSG_LASER;
                break;
            case PHOTON_TORPEDO:
                server_action_str = STR_MSG_PHOTON_TORPEDO;
                break;
            case SHIELDS_UP:
                server_action_str = STR_MSG_ESCUDOS;
                break;
            case CLOAKING:
                server_action_str = STR_MSG_CLOAKING;
                break;
            default:
                server_action_str = NULL;
                break;
            }
            
            if (!(server_action_str && user_action_str))
            {
                return 1;
            }
            
            printf(STR_MSG_TEMPLATE, user_action_str, server_action_str, bm.message, bm.client_hp, bm.server_hp);
            break;
        case MSG_INIT:
        case MSG_GAME_OVER:
            printf("%s", bm.message);
            break;
        case MSG_ESCAPE: 
            printf("%s%s%s%s%s", 
                (bm.client_action == HYPER_JUMP ? "Você " : ""), 
                (bm.client_action == HYPER_JUMP ? STR_MSG_HYPER_JUMP : ""), 
                (bm.server_action == HYPER_JUMP ? "Servidor " : ""), 
                (bm.server_action == HYPER_JUMP ? STR_MSG_HYPER_JUMP : ""), 
                bm.message);
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
