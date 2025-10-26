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

void error_handling(const char* errmsg, const char* func, int line)
{
    if (!(errmsg && func)) { return; }

    if (errno) { fprintf(stderr, "%s\nerrno : %d, strerr : %s\nLocation : %s():%d\n", errmsg, errno, strerror(errno), func, line); }
    else { fprintf(stderr, "%s\nLocation : %s():%d\n", errmsg, func, line); }
}

ssize_t send_reliable(int sock, const void* buf, size_t buf_size, int flag)
{
    ssize_t send_size;
    ssize_t send_size_sum = 0;
    const char* buf_current_loc = (const char*)buf;
    
    if (buf == NULL)
    {
        return 0;
    }

    while (send_size_sum < buf_size)
    {
        send_size = send(sock, buf_current_loc, buf_size - send_size_sum, flag);
        if (send_size < 0)
        {
            return send_size;
        }
        
        send_size_sum += send_size;
        buf_current_loc += send_size;
    }
    
    return buf_size;
}

ssize_t recv_reliable(int sock, void* buf, size_t buf_size, int flag)
{
    ssize_t recv_size;
    ssize_t recv_size_sum = 0;
    char* buf_current_loc = (char*)buf;
    
    if (buf == NULL)
    {
        return 0;
    }

    while (recv_size_sum < buf_size)
    {
        recv_size = recv(sock, buf_current_loc, buf_size - recv_size_sum, flag);
        if (recv_size <= 0)
        {
            return recv_size;
        }
        
        recv_size_sum += recv_size;
        buf_current_loc += recv_size;
    }
    
    return buf_size;
}

char* get_str_start_point(const char* str, size_t str_size)
{
    const char* start_point = str;
    const char* iter = str;
    size_t cnt = 0;
    if (!str)   
    {
        return NULL;
    }

    while (*iter != '\0' && cnt <= str_size)
    {
        iter++;
        cnt++;
        switch (*start_point)
        {
        case '\n':
        case ' ':
        case '\t':
            start_point++;
            break;
        }
    }
    
    return (char*)start_point; 
}