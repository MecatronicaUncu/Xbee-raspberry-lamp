/*
 *  Zigbee.c
 *  
 *
 *  Created by Alvaro Rodrigo Alonso Bivou on 05/04/13.
 *  Copyright 2013 ENIB. All rights reserved.
 *
 */


//----------------------------------------------------------------------------

#include "Zigbee.h"

#define CONFIG	1
#define DEBUG	1

//Message Id
int id=10;

//Init Circular list
msg *msg_list=NULL;

//Serial Port File Descriptor
int serialFd=0;


int
main(int argc,
     char **argv)
{
	//Circular list element
//	zigbee *zgb_elem=NULL;
	msg *msg_elem=NULL;

	//buffer
//	unsigned char buffer[0x100];
	
	 // default Config
	int portNumber=INPORT;
	char serialport[256];
	int baudrate = B9600; 
    
    //Init Mutex
    //sem_init(&mutex, 0, 1);
    
#if CONFIG
	//Check command line arguments
	if(argc<2)
	{fprintf(stderr,"Usage: %s listenport serialport baudrate\n",argv[0]); exit(1); }
	//... extract listenport number
	if(sscanf(argv[1],"%d",&portNumber)!=1){fprintf(stderr,"invalid listenport %s\n",argv[1]); exit(1); }
	// ... extract serialport number
	if(sscanf(argv[2],"%s",serialport)!=1)
	{fprintf(stderr,"invalid serialport %s\n",argv[1]); exit(1); }
	// ... extract baudrate number Optional
	if(argv[3]!=NULL){if(sscanf(argv[3],"%d",&baudrate)!=1)
	{fprintf(stderr,"invalid listenport %s\n",argv[2]);exit(1);}}
	
	//Init serial port
	serialFd = serial_init(serialport, baudrate);
	if(serialFd==-1) {fprintf(stderr,"invalid serialport %s\n",argv[2]); exit(1); }
#endif
	
	//Init Listen Socket	
	//---- create listen socket ----
	int listenSocket=socket(PF_INET,SOCK_STREAM,0);
	if(listenSocket==-1)
	{ perror("socket"); exit(1); }
	// ... avoiding timewait problems (optional)
	int on=1;
	if(setsockopt(listenSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int))==-1)
	{ perror("setsockopt"); exit(1); }
	// ... bound to any local address on the specified port
	struct sockaddr_in myAddr;
	myAddr.sin_family=AF_INET;
	myAddr.sin_port=htons(portNumber);
	myAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(listenSocket,(struct sockaddr *)&myAddr,sizeof(myAddr))==-1)
	{ perror("bind"); exit(1); }
	// ... listening connections
	if(listen(listenSocket,10)==-1)
	{ perror("listen"); exit(1); }
	
	
	//SELECT-System Call 
	fd_set rfds;
	for(;;)
	{	  
		FD_ZERO(&rfds);
		FD_SET(listenSocket,&rfds);
		FD_SET(serialFd,&rfds);
		int max_fd = (listenSocket > serialFd ? listenSocket : serialFd) + 1;
		
		// ... Do the select 
		select(max_fd, &rfds, NULL, NULL, NULL);
		
		// ... We have Serial input 
		if (FD_ISSET(serialFd, &rfds))
		{
			unsigned char *buf=(unsigned char*)malloc(0X100);
			int n=read(serialFd,buf,0x100);
			if(n<0){continue;}//Nothing read
			//Mostar el mensaje
			printf("%s",buf);
			//Reponse
			reponse(buf,n);
			//Free
			free(buf);
		}
		
		// ... We have listenSocket input
		else if (FD_ISSET(listenSocket, &rfds))
		{
			//Accept new connection
			struct sockaddr_in fromAddr;
			socklen_t len=sizeof(fromAddr);
			int dialogSocket=accept(listenSocket,(struct sockaddr *)&fromAddr,&len);
			//...no Connection
			if(dialogSocket==-1){if(errno!=EINTR){ perror("accept"); exit(1); }}
			//...succesful Connection
			else
			{
				//Cantidad Maxima?
				id++;
				//Print the new connection
				printf("new connection-> ID:%d from %s:%d \n",id,inet_ntoa(fromAddr.sin_addr),ntohs(fromAddr.sin_port));	
								
				//add msg to circular-list
				if ((msg_elem = (msg*)malloc(sizeof(msg))) == NULL) exit(-1);
				msg_elem->ident=id;
				msg_elem->sockFd=dialogSocket;
				HASH_ADD_INT(msg_list, ident, msg_elem);
				//---- start a new dialog thread ----
				  pthread_t th;
				  int *arg=(int *)malloc(sizeof(int));
				  *arg=dialogSocket;
				  if(pthread_create(&th,(pthread_attr_t *)0,dialogThread,arg))
					{ fprintf(stderr,"cannot create thread\n"); exit(1); }
			}
		}
	}
	//---- close listen socket ----
	close(listenSocket);
	//---- close serial socket ----
	close(serialFd);
	return 0;
}


void 
reponse(unsigned char * buf,int n)
{	
	msg *msg_elem=NULL;
	for(msg_elem=msg_list; msg_elem != NULL; msg_elem=(msg*)(msg_elem->hh.next))
	{
	      if(send(msg_elem->sockFd,buf,n,0)==-1)
	      { perror("send"); exit(1); }
	}
	return;
}

void *
dialogThread(void *arg)
{
  pthread_detach(pthread_self());
  //---- obtain dialog socket from arg ----
  int dialogSocket=*(int*)arg;
  free(arg);
  for(;;)
  {
      //---- receive and display message from client ----
      char buffer[0x100];
      int nb=recv(dialogSocket,buffer,0x100,0);
      if(nb<=0) { break; }
      buffer[nb]='\0';
      printf("%s\n",buffer);
      write(serialFd,buffer,nb);

      //---- send reply to client ----
      nb=sprintf(buffer,"%d bytes received\n",nb);
      if(send(dialogSocket,buffer,nb,0)==-1)
	{ perror("send"); exit(1); }
   }

  //---- close dialog socket ----
  printf("client disconnected\n");
  close(dialogSocket);
  return (void *)0;
}


//----------------------------------------------------------------------------
