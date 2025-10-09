#ifndef _STRUCTURES_H 
#define _STRUCTURES_H

#define MSG_SIZE 256

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
    IPV4,
    IPV6
} IPType;

typedef enum
{
    LASER_ATTACK,
    PHOTON_TORPEDO,
    SHIELDS_UP,
    CLOACKING,
    HYPER_JUMP,
    ACTION_CNT
} Action;

typedef enum
{
    ERR_SOCKET = 1,
    ERR_BIND,
    ERR_CONNECT,
    ERR_LISTEN,
    ERR_ACCEPT
} Errors;

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
    int hp;
    int action;
} Entity;

#endif