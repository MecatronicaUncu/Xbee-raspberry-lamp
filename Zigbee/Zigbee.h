#include <inttypes.h>
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


#include "Serial_Init.h"
#include "uthash.h"

#define INPORT  6789

#if !defined(_ZIGBEE_H)
#define _ZIGBEE_H
/*
 * Structures
 */
//Message Struture
typedef struct msg_struct {
    int ident;            /* we'll use this field as the key */ 
    int sockFd;        //Parent Socket 
    UT_hash_handle hh; /* makes this structure hashable */
}msg;

/*
 * Functions
 */ 
//Thread function
void *
dialogThread(void *arg);
//Function for return values to the webpage
void 
reponse(unsigned char *buf,int n);

#endif
