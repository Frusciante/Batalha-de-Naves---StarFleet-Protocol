#include "util_all.h"

char* strncpy_safer(char* dest, const char* src, size_t n)     
{
    strncpy(dest, src, n);
    dest[n - 1] = '\0';
    return dest;
}

void error_handling(const char* errmsg, int errno_exists, int shutdown_program)
{
    if (!errmsg) { exit(1); }

    if (errno_exists) { fprintf(stderr, "%s, errno : %d, strerr : %s\n", errmsg, errno, strerror(errno)); }
    else { fputs(errmsg, stderr); }

    if (shutdown_program) { exit(1); }
}