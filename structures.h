#ifndef _STRUCTURES_H 
#define _STRUCTURES_H

#define MSG_SIZE 256
#define ERR_STRING_LEN 256

#define MSG_OF_WELCOME "Conectado ao servidor.\nSua nave: SS-42 Voyager (HP: 100)\n"
#define MSG_OF_LASER "Você disparou um Lazer!"
#define MSG_OF_PHOTON_TORPEDO "Você disparou um Photon Torpedo!"
#define MSG_OF_ESCUDOS "Você ativou os Escudos!"
#define MSG_OF_CLOAKING "Você ativou Cloaking!"
#define MSG_OF_HYPER_JUMP "Você acionou Hyper Jump!"


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
} Inventory;


#endif