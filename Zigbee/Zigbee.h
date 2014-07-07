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
#include <semaphore.h>


#include "Serial_Init.h"
#include "APIframe_gen.h"
#include "uthash.h"

#define INPORT  6789

//API Identifier 
#define ZBTRANSTAT	0x8B //ZigBee Transmit Status
#define DELIVERY	0x8
#define SUCCESS		0x00

#define ATRESPONSE	0x88 // AT Response
#define RATRESPONSE	0x97

#define ZBRECVPCK	0x90 //Zigbee Receive Packet
#define NODEID		0x95

//Web Page
#define ATDIV		0x10
#define ALERT 		0xFE
#define	NEWSENSOR	0xFF


#if !defined(_ZIGBEE_H)
#define _ZIGBEE_H
/*
 * Structures
 */
//Message Struture
typedef struct msg_struct {
    int ident;            /* we'll use this field as the key */ 
    int sockFd;        //Parent Socket 
    int position;      //Position of the msg on the webpage
    UT_hash_handle hh; /* makes this structure hashable */
}msg;

//Zigbee Structure
typedef struct zigbeee_struct {
    unsigned char address[8];
    unsigned char network[2];            
    int key;       /* we'll use this field as the key */      
    UT_hash_handle hh; /* makes this structure hashable */
}zigbee;

/*
 * Functions
 */ 
//Thread function
void *
send_serial(void* arg);
//Function for return values to the webpage
void 
reponse(unsigned char *buf,int n);

#define CONCAT(s) str(s)
#define str(s)		#s


#endif
