#include "util_common.h"

const int OPT_ON = 1;
const int KEEP_IDLE_TIME = 3;
const int KEEP_INTERVAL = 2;
const int KEEP_CNT = 2;

char* strncpy_safer(char* dest, const char* src, size_t n)     
{
    strncpy(dest, src, n);
    dest[n - 1] = '\0';
    return dest;
}

void error_handling(const char* errmsg, int errno_exists, int shutdown_program)
{
    if (!errmsg) { exit(1); }

    if (errno_exists) { fprintf(stderr, "%s\nerrno : %d, strerr : %s\n", errmsg, errno, strerror(errno)); }
    else { fputs(errmsg, stderr); }

    if (shutdown_program) { exit(1); }
}