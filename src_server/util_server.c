#include "util_server.h"

static const char* const STR_MSG_WELCOME = "Conectado ao servidor.\nSua nave: SS-42 Voyager (HP: 100)\n";
static const char* const STR_MSG_LASER = "disparou um Lazer!\n";
static const char* const STR_MSG_PHOTON_TORPEDO = "disparou um Photon Torpedo!\n";
static const char* const STR_MSG_ESCUDOS = "ativou os Escudos!\n";
static const char* const STR_MSG_CLOAKING = "ativou Cloaking!\n";
static const char* const STR_MSG_HYPER_JUMP = "acionou Hyper Jump!\n";
static const char* const STR_MSG_RUN = "escapou para o hiperespaço.\n";
static const char* const STR_MSG_ACTION_SELECTION = "\nEscolha sua ação\n0 - Laser Attack\n1 - Photon Torpedo\n2 - Shields Up\n3 - Cloaking\n4 - Hyper Jump\n\n> ";
static const char* const STR_MSG_TEMPLATE = "Você %sServidor %sResultado: %sPlacar: Você %d x %d Inimigo\n";

int init_socket(int address_family, unsigned short target_port)
{
    int sock = socket(address_family, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr_v4 = {};
    struct sockaddr_in6 serv_addr_v6 = {};
    struct sockaddr *serv_addr = NULL;

    if (-1 == sock) { return 1; }

    switch (address_family)
    {
    case AF_INET:
        serv_addr_v4.sin_family = AF_INET;
        serv_addr_v4.sin_port = htons(target_port);
        serv_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr = (struct sockaddr *)&serv_addr_v4;
        break;

    case AF_INET6:
        serv_addr_v6.sin6_family = AF_INET6;
        serv_addr_v6.sin6_port = htons(target_port);
        serv_addr_v6.sin6_addr = in6addr_any;
        serv_addr = (struct sockaddr *)&serv_addr_v6;
        break;
    }

    if (!serv_addr) { return -1; }
    if (-1 == bind(sock, serv_addr, (address_family == AF_INET ? sizeof(serv_addr_v4) : sizeof(serv_addr_v6)))){ return 1; }
    if (-1 == listen(sock, BACKLOG_SIZE)){ return 1; }

    return sock;
}

int battle(int sock)
{
    BattleMessage bm = {};
    Inventory inventory = {100, 100, 0, 0, 0};
    bm.type = MSG_INIT;
    strncpy_safer(bm.message, STR_MSG_WELCOME, sizeof(bm.message));
    if (-1 == send(sock, (void *)&bm, sizeof(bm), 0))
    {
        error_handling(ERR_CONNECTION, 1, 0);
        return 1;
    }

    while (bm.type != MSG_ESCAPE && bm.type != MSG_GAME_OVER)
    {
        bm.type = MSG_ACTION_REQ;
        strncpy_safer(bm.message, STR_MSG_ACTION_SELECTION, sizeof(bm.message));
        if (-1 == send(sock, (void *)&bm, sizeof(bm), 0))
        {
            error_handling(ERR_CONNECTION, 1, 0);
            return 1;
        }
        
        switch (recv(sock, &bm, sizeof(bm), 0))
        {
        case 0:
            close(sock);
            error_handling("Lost connection to the client.\n", 0, 0);
            return 1;
        case -1:
            close(sock);
            error_handling(ERR_CONNECTION, 1, 0);
            return 1;
        }

        bm.type = MSG_BATTLE_RESULT;
        if (bm.client_action < 0 || bm.client_action > 4) 
        {
            strncpy_safer(bm.message, ERR_MSG, sizeof(bm.message));
        }
        else
        {
            ++inventory.total_turns;
            bm.server_action = rand() % ACTION_CNT;
            
            if (-1 == get_battle_result(&bm, &inventory)) { return 1; }
            if ((inventory.client_hp <= 0 || inventory.server_hp <= 0) || bm.type == MSG_ESCAPE)
            {
                if (bm.type != MSG_ESCAPE)
                {
                    bm.type = MSG_GAME_OVER;
                }
            }
        }
        if (-1 == send(sock, &bm, sizeof(bm), 0))
        {
            error_handling(ERR_CONNECTION, 1, 0);
            return 1;
        }
    }

    strncpy_safer(bm.message, "\nFim de Jogo!", sizeof(bm.message));
    if (-1 == send(sock, &bm, sizeof(bm), 0))
    {
        error_handling(ERR_CONNECTION, 1, 0);
        return 1;
    }

    if (bm.type == MSG_GAME_OVER)
    {
        strncpy_safer(bm.message,
                    (inventory.client_hp > inventory.server_hp ? "Você derrotou a frota inimiga!\n" : 
                    (inventory.client_hp < inventory.server_hp ? "Sua nave foi destruída!\n" : "Ambos foram destruída!\n")), sizeof(bm.message));
    }
    else
    {
        strncpy_safer(bm.message, "Obrigado por jogar!\n", sizeof(bm.message));
    }
    bm.type = MSG_INVENTORY;
    bm.client_shields = inventory.client_shields;
    bm.server_hp = inventory.server_hp;
    bm.client_hp = inventory.client_hp;
    bm.client_torpedoes = inventory.client_torpedoes;
    bm.client_action = inventory.total_turns;
    if (-1 == send(sock, &bm, sizeof(bm), 0))
    {
        error_handling(ERR_CONNECTION, 1, 0);
        return 1;
    }

    return 0;
}

int get_battle_result(BattleMessage *bm, Inventory *inventory)
{
    int result = 0;
    if (!(bm && inventory))
    {
        return -1;
    }

    if (bm->client_action == HYPER_JUMP || bm->server_action == HYPER_JUMP)
    {
        snprintf(bm->message, sizeof(bm->message),
                 "%s%s%s%s%s%s%s%s",
                 (bm->client_action == HYPER_JUMP ? "Você " : ""), (bm->client_action == HYPER_JUMP ? STR_MSG_HYPER_JUMP : ""),
                 (bm->server_action == HYPER_JUMP ? "Servidor " : ""), (bm->server_action == HYPER_JUMP ? STR_MSG_HYPER_JUMP : ""),
                 (bm->client_action == HYPER_JUMP ? "Sua nave " : ""), (bm->client_action == HYPER_JUMP ? STR_MSG_RUN : ""),
                 (bm->server_action == HYPER_JUMP ? "Nave inimiga " : ""), (bm->server_action == HYPER_JUMP ? STR_MSG_RUN : ""));
        bm->type = MSG_ESCAPE;

        return 0;
    }

    switch (bm->client_action)
    {
    case LASER_ATTACK:
        switch (bm->server_action)
        {
        case SHIELDS_UP:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_ESCUDOS, "Seu ataque bloqueado!\n", inventory->client_hp, inventory->server_hp);
            break;
        case CLOACKING:
            inventory->server_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_CLOAKING, "Acerto! Nave inimiga perdeu 20 HP.\n", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_PHOTON_TORPEDO, "Você recebeu 20 de dano.\n", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_LASER, "Ambos receberam 20 de dano.\n", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    case PHOTON_TORPEDO:
        ++inventory->client_torpedoes;
        switch (bm->server_action)
        {
        case SHIELDS_UP:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_ESCUDOS, "Seu ataque bloqueado!\n", inventory->client_hp, inventory->server_hp);
            break;
        case CLOACKING:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_CLOAKING, "Seu ataque falhou!\n", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->server_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_LASER, "Acerto! Nave inimiga perdeu 20 HP.\n", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_PHOTON_TORPEDO, "Ambos receberam 20 de dano.\n", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    case SHIELDS_UP:
        ++inventory->client_shields;
        switch (bm->server_action)
        {
        case SHIELDS_UP:
        case CLOACKING:
            snprintf(bm->message,
                     sizeof(bm->message),
                     STR_MSG_TEMPLATE, STR_MSG_ESCUDOS, (bm->server_action == SHIELDS_UP ? STR_MSG_ESCUDOS : STR_MSG_CLOAKING), "Aconteceu nada.\n", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
        case PHOTON_TORPEDO:
            snprintf(bm->message,
                     sizeof(bm->message),
                     STR_MSG_TEMPLATE, STR_MSG_ESCUDOS, (bm->server_action == LASER_ATTACK ? STR_MSG_LASER : STR_MSG_PHOTON_TORPEDO), "Ataque inimigo bloqueado!\n", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    case CLOACKING:
        switch (bm->server_action)
        {
        case SHIELDS_UP:
        case CLOACKING:
            snprintf(bm->message,
                     sizeof(bm->message),
                     STR_MSG_TEMPLATE, STR_MSG_CLOAKING, (bm->server_action == SHIELDS_UP ? STR_MSG_ESCUDOS : STR_MSG_CLOAKING), "Aconteceu nada.\n", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_CLOAKING, STR_MSG_PHOTON_TORPEDO, "Ataque inimigo falhou!\n", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_CLOAKING, STR_MSG_LASER, "Você recebeu 20 de dano.\n", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    }

    bm->type = MSG_BATTLE_RESULT;

    return 0;
}