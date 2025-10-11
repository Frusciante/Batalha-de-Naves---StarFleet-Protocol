#include "util_server.h"

static const char* const STR_MSG_WELCOME = "Conectado ao servidor.\nSua nave: SS-42 Voyager (HP: 100)\n";
static const char* const STR_MSG_LASER = "disparou um Lazer!\n";
static const char* const STR_MSG_PHOTON_TORPEDO = "disparou um Photon Torpedo!\n";
static const char* const STR_MSG_ESCUDOS = "ativou os Escudos!\n";
static const char* const STR_MSG_CLOAKING = "ativou Cloaking!\n";
static const char* const STR_MSG_HYPER_JUMP = "acionou Hyper Jump!\n";
static const char* const STR_MSG_RUN = "escapou para o hiperespaço\n";
static const char* const STR_MSG_ACTION_SELECTION = "Escolha sua ação\n0 - Laser Attack\n1 - Photon Torpedo\n2 - Shields Up\n3 - Cloaking\n4 - Hyper Jump\n\n> ";
static const char* const STR_MSG_TEMPLATE = "Você %sServidor %sResultado: %sPlacar: Você %d x %d Inimigo";

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
    BattleMessage battle_message = {};
    Inventory inventory = {};
    battle_message.type = MSG_INIT;
    strncpy_safer(battle_message.message, STR_MSG_WELCOME, sizeof(battle_message.message));
    send(sock, (void *)&battle_message, sizeof(battle_message), 0);

    for (;;)
    {
        battle_message.type = MSG_ACTION_REQ;
        strncpy_safer(battle_message.message, STR_MSG_ACTION_SELECTION, sizeof(battle_message.message));
        send(sock, (void *)&battle_message, sizeof(battle_message), 0);
        recv(sock, (void *)&battle_message, sizeof(battle_message), 0);
        
        battle_message.type = MSG_BATTLE_RESULT;
        if (battle_message.client_action < 0 || battle_message.client_action > 4) 
        {
            strncpy_safer(battle_message.message, "Erro: escolha inválida!\nPor favor selecione um valor entre 0 a 4.\n\n", sizeof(battle_message.message));
        }
        else
        {
            ++inventory.total_turns;
            battle_message.server_action = rand() % ACTION_CNT;
            if (-1 == get_battle_result(&battle_message, &inventory)) { return 1; }
            if (!(inventory.client_hp > 0 && inventory.server_hp > 0) || battle_message.type == MSG_ESCAPE)
            {
                if (battle_message.type != MSG_ESCAPE)
                {
                    strncpy_safer(battle_message.message,
                                (battle_message.client_hp > battle_message.server_hp ? "Você derrotou a frota inimiga!" : 
                                (battle_message.client_hp < battle_message.server_hp ? "Sua nave foi destruída!" : "Ambos foram destruída!")), sizeof(battle_message));
                    battle_message.type = MSG_GAME_OVER;
                }
                break;
            }

            send(sock, &battle_message, sizeof(battle_message), 0);
        }
    }
    
    send(sock, &battle_message, sizeof(battle_message), 0);

    battle_message.type = MSG_INVENTORY;
    battle_message.client_shields = inventory.client_shields;
    battle_message.server_hp = inventory.server_hp;
    battle_message.client_hp = inventory.client_hp;
    battle_message.client_torpedoes = inventory.client_shields;
    battle_message.client_action = inventory.total_turns;

    send(sock, &battle_message, sizeof(battle_message), 0);

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
                 "%s%s%s%s%s%s%s%s\n",
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
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_ESCUDOS, "Seu ataque bloqueado!", inventory->client_hp, inventory->server_hp);
            break;
        case CLOACKING:
            inventory->server_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_ESCUDOS, "Acerto! Nave inimiga perdeu 20 HP.", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_PHOTON_TORPEDO, "Você recebeu 20 de dano.", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_LASER, STR_MSG_LASER, "Ambos receberam 20 de dano.", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    case PHOTON_TORPEDO:
        ++inventory->client_torpedoes;
        switch (bm->server_action)
        {
        case SHIELDS_UP:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_ESCUDOS, "Seu ataque bloqueado!", inventory->client_hp, inventory->server_hp);
            break;
        case CLOACKING:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_CLOAKING, "Seu ataque falhou!", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->server_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_LASER, "Acerto! Nave inimiga perdeu 20 HP.", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            inventory->server_hp -= 20;
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_PHOTON_TORPEDO, STR_MSG_PHOTON_TORPEDO, "Ambos receberam 20 de dano.", inventory->client_hp, inventory->server_hp);
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
                     STR_MSG_TEMPLATE, STR_MSG_ESCUDOS, (bm->server_action == SHIELDS_UP ? STR_MSG_ESCUDOS : STR_MSG_CLOAKING), "Aconteceu nada.", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
        case PHOTON_TORPEDO:
            snprintf(bm->message,
                     sizeof(bm->message),
                     STR_MSG_TEMPLATE, STR_MSG_ESCUDOS, (bm->server_action == LASER_ATTACK ? STR_MSG_LASER : STR_MSG_PHOTON_TORPEDO), "Ataque inimigo bloqueado!", inventory->client_hp, inventory->server_hp);
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
                     STR_MSG_TEMPLATE, STR_MSG_CLOAKING, (bm->server_action == SHIELDS_UP ? STR_MSG_ESCUDOS : STR_MSG_CLOAKING), "Aconteceu nada.", inventory->client_hp, inventory->server_hp);
            break;
        case PHOTON_TORPEDO:
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_CLOAKING, STR_MSG_PHOTON_TORPEDO, "Ataque inimigo falhou!", inventory->client_hp, inventory->server_hp);
            break;
        case LASER_ATTACK:
            inventory->client_hp -= 20;
            snprintf(bm->message, sizeof(bm->message), STR_MSG_TEMPLATE, STR_MSG_CLOAKING, STR_MSG_LASER, "Você recebeu 20 de dano.", inventory->client_hp, inventory->server_hp);
            break;
        }
        break;
    }

    bm->type = MSG_BATTLE_RESULT;

    return 0;
}