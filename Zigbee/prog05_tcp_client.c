//----------------------------------------------------------------------------

#include "crsUtils.h"

int
main(int argc,
     char **argv)
{
//---- check command line arguments ----
if(argc!=3)
  { fprintf(stderr,"usage: %s destination port\n",argv[0]); exit(1); }

//---- extract destination IP address ----
struct hostent *host=gethostbyname(argv[1]);
if(!host)
  { fprintf(stderr,"unknown host %s\n",argv[1]); exit(1); }
in_addr_t ipAddress=*((in_addr_t *)(host->h_addr));

//---- extract destination port number ----
int portNumber;
if(sscanf(argv[2],"%d",&portNumber)!=1)
  { fprintf(stderr,"invalid port %s\n",argv[2]); exit(1); }

//---- create client socket ----
int clientSocket=socket(PF_INET,SOCK_STREAM,0);
if(clientSocket==-1)
  { perror("socket"); exit(1); }
// ... connected to the specified destination/port
struct sockaddr_in toAddr;
toAddr.sin_family=AF_INET;
toAddr.sin_port=htons(portNumber);
toAddr.sin_addr.s_addr=ipAddress;
if(connect(clientSocket,(struct sockaddr *)&toAddr,sizeof(toAddr))==-1)
  { perror("connect"); exit(1); }

#if 0
  for(;;)
    {
    //---- read next line on standard input ----
    char buffer[0x100];
    if(!fgets(buffer,0x100,stdin)) { break; } // [Control]+[d] --> EOF
  
    //---- send message to server ----
    if(send(clientSocket,buffer,strlen(buffer),0)==-1)
      { perror("send"); exit(1); }
  
    //---- receive and display reply ----
    int nb=recv(clientSocket,buffer,0x100,0);
    if(nb<=0) { break; } // EOF
    buffer[nb]='\0';
    printf("%s\n",buffer);
    }
#else
  for(;;)
    {
    char buffer[0x100];
    //---- wait for incoming informations ----
    fd_set rdSet;
    FD_ZERO(&rdSet);
    FD_SET(0,&rdSet); // standard input
    FD_SET(clientSocket,&rdSet);
    int maxFd=clientSocket;
    if(select(maxFd+1,&rdSet,(fd_set *)0,(fd_set *)0,(struct timeval *)0)==-1)
      { perror("select"); exit(1); }

    //---- read next line on standard input (if available) ----
    if(FD_ISSET(0,&rdSet))
      {
      if(!fgets(buffer,0x100,stdin)) { break; } // [Control]+[d] --> EOF
      //---- send message to server ----
      if(send(clientSocket,buffer,strlen(buffer),0)==-1)
        { perror("send"); exit(1); }
      }
  
    //---- receive and display reply (if available) ----
    if(FD_ISSET(clientSocket,&rdSet))
      {
      int nb=recv(clientSocket,buffer,0x100,0);
      if(nb<=0) { break; } // EOF
      buffer[nb]='\0';
      printf("%s",buffer); fflush(stdout);
      }
    }
#endif

//---- close client socket ----
close(clientSocket);
return 0;
}

//----------------------------------------------------------------------------
