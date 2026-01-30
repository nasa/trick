
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// message_publish when you don't have a message publisher class
#define MAX_MSG_HEADER_SIZE 256
#define MAX_MSG_SIZE 20480

extern "C" int message_publish_standalone(int level, const char * format_msg, ...)
{
    char msg_buf[MAX_MSG_SIZE];
    char date_buf[MAX_MSG_HEADER_SIZE];
    char hostname[64];
    time_t date;
    // timeval contains both tv_sec and tv_usec
    // tv_sec represents seconds since the epoch and is used for time stamp without sub-second.
    // tv_usec are microseconds past the last second and is used for printing out sub-second.
    struct timeval time_val;

    va_list args;

    va_start(args, format_msg);
    (void)vsnprintf(msg_buf, MAX_MSG_SIZE, format_msg, args);
    va_end(args);

    gettimeofday(&time_val, NULL);

    // tv_sec represents seconds since the epoch
    date = time_val.tv_sec;

    strftime(date_buf, (size_t)20, "%Y/%m/%d,%H:%M:%S", localtime(&date));
    (void)gethostname(hostname, (size_t)48);
    // use %lu for tv_usec
    fprintf(stdout,
            "|L %d|%s.%06lu|  |%s|T %d|%.2f| %s",
            level,
            // so that we don't call any exec routines, use process id 0 and sim time 0.0
            // cast tv_usec to unsigned long to avoid potential warning
            date_buf,
            (unsigned long)time_val.tv_usec,
            hostname,
            0,
            0.0,
            msg_buf);
    fflush(stdout);

    return (0);
}
