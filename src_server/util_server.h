#ifndef _UTIL_SERVER_H
#define _UTIL_SERVER_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"
#include <sys/socket.h>
#include <arpa/inet.h>

int get_random_opt(void);
int init_socket(int address_family, unsigned short target_port);
    
#endif