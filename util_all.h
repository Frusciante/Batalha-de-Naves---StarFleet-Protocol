#ifndef _UTIL_ALL_H 
#define _UTIL_ALL_H

#define MSG_SIZE 256
#define ERR_STRING_LEN 256
#define ERR_MSG "Erro: escolha inválida!\nPor favor selecione um valor entre 0 a 4.\n"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum 
{
    MSG_INIT,           // Boas vindas
    MSG_ACTION_REQ,     // Solicitação de ação
    MSG_ACTION_RES,     // Cliente responde com ação escolhida
    MSG_BATTLE_RESULT,  // Resultado do turno
    MSG_INVENTORY,      // Atualização de inventário
    MSG_GAME_OVER,      // Fim de jogo
    MSG_ESCAPE          // Jogador fugiu
} MessageType;

typedef enum
{
    LASER_ATTACK,
    PHOTON_TORPEDO,
    SHIELDS_UP,
    CLOACKING,
    HYPER_JUMP,
    ACTION_CNT
} Action;

typedef struct 
{
    int type;
    int client_action;
    int server_action;
    int client_hp;
    int server_hp;
    int client_torpedoes;
    int client_shields;
    char message[MSG_SIZE];
} BattleMessage;

typedef struct
{
    int client_hp;
    int server_hp;
    int client_torpedoes;
    int client_shields;
    int total_turns;
} Inventory;

char* strncpy_safer(char* dest, const char* src, size_t n);

void error_handling(const char* errmsg, int errno_exists, int shutdown_program);

#endif