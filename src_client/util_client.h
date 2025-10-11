#ifndef _UTIL_CLIENT_H
#define _UTIL_CLIENT_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../src_common/util_common.h"


int init_socket(const char* ip_str, const char* port_str);
int battle(int sock);

#endif