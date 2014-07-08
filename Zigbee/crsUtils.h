//----------------------------------------------------------------------------

#ifndef CRSUTILS_H
#define CRSUTILS_H 1

#if !defined __cplusplus
# include <stdbool.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>

inline static
double // time in seconds
getTime(void)
{
struct timeval tv;
gettimeofday(&tv,(struct timezone *)0);
return tv.tv_sec+1e-6*tv.tv_usec;
}

#endif // CRSUTILS_H

//----------------------------------------------------------------------------
