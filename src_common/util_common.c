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

void error_handling(const char* errmsg, const char* func, int line, int errno_exists)
{
    if (!(errmsg && func)) { return; }

    if (errno_exists) { fprintf(stderr, "%s\nerrno : %d, strerr : %s\nLocation : %s():%d\n", errmsg, errno, strerror(errno), func, line); }
    else { fprintf(stderr, "%s\nLocation : %s():%d\n", errmsg, func, line); }
}