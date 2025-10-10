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

#include "structures.h"

#define BACKLOG_SIZE 10

void error_handling(const char* errmsg, int errno_exists, int shutdown_program);
int init_socket(int address_family, unsigned short target_port);
int battle(int sock);
int get_battle_result(BattleMessage* bm, Inventory* inventory);

#endif