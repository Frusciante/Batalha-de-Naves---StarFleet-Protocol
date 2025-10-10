#include "util_server.h"

char* strncpy_safer(char* dest, const char* src, size_t n)
{
    strncpy(dest, src, n);
    dest[n - 1] = '\0';
    return dest;
}

void error_handling(const char* errmsg, int errno_exists, int shutdown_program)
{
    if (!errmsg) { exit(1); }

    if (errno_exists) { fprintf(stderr, "%s, errno : %d, strerr : %s", errmsg, errno, strerror(errno)); }
    else { fputs(errmsg, stderr); }

    if (shutdown_program) { exit(1); }
}

int init_socket(int address_family, unsigned short target_port)
{
    int sock = socket(address_family, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr_v4 = {};
    struct sockaddr_in6 serv_addr_v6 = {};
    struct sockaddr* serv_addr = NULL; 

    if (-1 == sock) { return 1; }

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
        serv_addr_v6.sin6_port = htons(target_port);
        serv_addr_v6.sin6_addr = in6addr_any;
        serv_addr = (struct sockaddr*)&serv_addr_v6;
        break;    
    }
    
    if (!serv_addr) { return -1; }

    if (-1 == bind(sock, serv_addr, (address_family == AF_INET ? sizeof(serv_addr_v4) : sizeof(serv_addr_v6)))) { return 1; }

    if (-1 == listen(sock, BACKLOG_SIZE)) { return 1; }
    
    return sock;
}

int battle(int sock)
{
    BattleMessage battle_message = {};
    Inventory inventory = {};
    battle_message.type = MSG_INIT;
    strncpy_safer(battle_message.message, MSG_OF_WELCOME, sizeof(battle_message.message));
    send(sock, (void*)&battle_message, sizeof(battle_message), 0);

    for (;;)
    {
        recv(sock, (void*)&battle_message, sizeof(battle_message), 0);
        battle_message.client_hp = inventory.client_hp;
        battle_message.server_hp = inventory.server_hp;
        battle_message.server_action = rand() % ACTION_CNT;

        if (-1 == get_battle_result(&battle_message, &inventory)) { return 1; }        
        send(sock, (void*)&battle_message, sizeof(battle_message), 0);
        if (battle_message.type == MSG_GAME_OVER) { break; }
    }

    battle_message.client_hp = inventory.client_hp;
    battle_message.server_hp = inventory.server_hp;
    battle_message.client_shields = inventory.client_shields;
    battle_message.client_torpedoes = inventory.client_torpedoes;

    send(sock, &battle_message, sizeof(battle_message), 0);

    return 0;
}

int get_battle_result(BattleMessage* bm, Inventory* inventory)
{
    int result = 0;
    if (!(bm && inventory))
    {
        return -1;    
    }
    
    if (bm->client_action == HYPER_JUMP || bm->server_action == HYPER_JUMP)
    {
        if (bm->client_action == bm->server_action) { strncpy_safer(bm->message, "Sua nave e a nave inimiga escaparam para o hiperspaço.", sizeof(bm->message)); }
        else if (bm->client_action == HYPER_JUMP && bm->server_action != HYPER_JUMP) {strncpy_safer(bm->message, "Sua nave escapou para o hiperspaço.", sizeof(bm->message)); }
        else { strncpy_safer(bm->message, "Nave inimiga escapou para o hiperspaço.", sizeof(bm->message)); }
        bm->type = MSG_GAME_OVER;

        return 0;
    }

    switch (bm->client_action)
    {
    case LASER_ATTACK:
        switch (bm->server_action)
        {
        case SHIELDS_UP:
            strncpy_safer(bm->message, "Seu ataque bloqueado!", sizeof(bm->message));
            break;
        case CLOACKING:
            strncpy_safer(bm->message, "Acerto! Nave inimiga perdeu 20 HP", sizeof(bm->message));
            bm->server_hp -= 20;
            inventory->server_hp -= 20;
            break;
        case PHOTON_TORPEDO:
            strncpy_safer(bm->message, "Você recebeu 20 de dano", sizeof(bm->message));
            bm->client_hp -= 20;
            inventory->client_hp -= 20;
            break;
        case LASER_ATTACK:
            strncpy_safer(bm->message, "Ambos receberam 20 de dano", sizeof(bm->message));
            bm->server_hp -= 20;
            bm->client_hp -= 20;
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            break;
        }
        break;
    case PHOTON_TORPEDO:
        ++inventory->client_torpedoes;
        switch (bm->server_action)
        {
        case SHIELDS_UP:
        case CLOACKING:
            strncpy_safer(bm->message, "Seu ataque bloqueado!", sizeof(bm->message));
            break;
        case LASER_ATTACK:
            strncpy_safer(bm->message, "Acerto! Nave inimiga perdeu 20 HP", sizeof(bm->message));
            bm->server_hp -= 20;
            inventory->server_hp -= 20;
            break;
        case PHOTON_TORPEDO:
            strncpy_safer(bm->message, "Ambos receberam 20 de dano", sizeof(bm->message));
            bm->server_hp -= 20;
            bm->client_hp -= 20;
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            break;
        }
        break;
    case SHIELDS_UP:
        ++inventory->client_shields;
        switch (bm->server_action)
        {
        case SHIELDS_UP:
        case CLOACKING:
            strncpy_safer(bm->message, "Aconteceu nada!", sizeof(bm->message));
            break;
        case LASER_ATTACK:
        case PHOTON_TORPEDO:
            strncpy_safer(bm->message, "Ataque inimigo bloqueado!", sizeof(bm->message));
            break;
        }
        break;
    case CLOACKING:
        switch (bm->server_action)
        {
        case SHIELDS_UP:
        case CLOACKING:
            strncpy_safer(bm->message, "Aconteceu nada!", sizeof(bm->message));
            break;
        case PHOTON_TORPEDO:
            strncpy_safer(bm->message, "Ataque inimigo falhou!", sizeof(bm->message));
            break;
        case LASER_ATTACK:
            strncpy_safer(bm->message, "Você recebeu 20 de dano", sizeof(bm->message));
            bm->client_hp -= 20;
            inventory->client_hp -= 20;
            break;
        }
        break;
    }

    if (!(bm->server_hp && bm->client_hp)) { bm->type = MSG_GAME_OVER; }
    else { bm->type = MSG_BATTLE_RESULT; };

    return 0;
}