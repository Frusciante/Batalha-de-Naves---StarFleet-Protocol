#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "util_client.h"

int main(int argc, char* argv[])
{
    char error_string[ERR_STRING_LEN] = {};
    int battle_result;
    int sock;

    setlocale(LC_ALL, "");
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
    {
        snprintf(error_string, sizeof(error_string), "Usage: %s <target ip (v4 or v6)> <target port>", argv[0]);
        error_handling(error_string, 0, 1);
    }

    sock = init_socket(argv[1], argv[2]);

    switch (sock)
    {
    case -4:
        snprintf(error_string, sizeof(error_string), "Format error. Port should be an integer between 1024 and 65535. Your input : %s", argv[2]);
        error_handling(error_string, 0, 1);
        break;
    case -3:
        snprintf(error_string, sizeof(error_string), "Foramt error. IP should be 'dotted-decimal(IPv4)' format or 'colon-hexadecimal(IPv6)' format. Your input : %s", argv[1]);
        error_handling(error_string, 0, 1);
        break;
    case -2:
        error_handling("IP or PORT is NULL", 0, 1);
        break;
    case -1:
        error_handling("Error occured while initizliaing a socket", 1, 1);
        break;
    }

    battle_result = battle(sock);
    switch (battle_result)
    {
    case 1:
        close(sock);
        error_handling("The game closed unexpectedly.\n", 0, 0);    
        break;
    
    case 0:
        close(sock);
        break;
    }
    
    return 0;
}
