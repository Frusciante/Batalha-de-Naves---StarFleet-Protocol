#ifndef _UTIL_SERVER_H
#define _UTIL_SERVER_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define STR_MSG_WELCOME "Conectado ao servidor.\nSua nave: SS-42 Voyager (HP: 100)\n"
#define STR_MSG_LASER "disparou um Lazer!\n"
#define STR_MSG_PHOTON_TORPEDO "disparou um Photon Torpedo!\n"
#define STR_MSG_ESCUDOS "ativou os Escudos!\n"
#define STR_MSG_CLOAKING "ativou Cloaking!\n"
#define STR_MSG_HYPER_JUMP "acionou Hyper Jump!\n"
#define STR_MSG_RUN "escapou para o hiperespaço\n" 
#define STR_MSG_ACTION_SELECTION "Escolha sua ação\n0 - Laser Attack\n1 - Photon Torpedo\n2 - Shields Up\n3 - Cloaking\n4 - Hyper Jump\n\n> "
#define STR_MSG_TEMPLATE "Você %sServidor %sResultado: %sPlacar: Você %d x %d Inimigo"

#include "../util_all.h"

#define BACKLOG_SIZE 10

int init_socket(int address_family, unsigned short target_port);
int battle(int sock);
int get_battle_result(BattleMessage* bm, Inventory* inventory);

#endif