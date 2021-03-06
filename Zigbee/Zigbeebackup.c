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
//Zigbee Id
char zgb_id=10;

//Init Circular list
msg 	 *msg_list=NULL;
zigbee  *zgb_list=NULL;

//Serial Port File Descriptor
int serialFd=0;

//Declare Mutex
//sem_t mutex;





int
main(int argc,
     char **argv)
{
	//Circular list element
	zigbee *zgb_elem=NULL;
	msg *msg_elem=NULL;

	//buffer
	unsigned char buffer[0x100];
	
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
			unsigned char *buf=(unsigned char*)malloc(HEADER+FRAME+ZBADDR+BUFFSIZE+CHECKSUM);
			int n=read(serialFd,buf,HEADER+FRAME+ZBADDR+BUFFSIZE+CHECKSUM);
			if(n<0){continue;}//Nothing read
			//Reponse
			reponse(buf,n);
			//Free
			free(buf);
		}
		
		//we have listenSocket input
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
				//send Zigbee list
				int nb=0;
				if(zgb_list==NULL){
#if DEBUG		
					nb=sprintf((char*)buffer,"Any Zigbee connected yet\n");
					if(send(dialogSocket,buffer,nb,0)==-1){ perror("send"); exit(1); }
#endif
					}
				else{//Report to the webpage all zigbee conected
					for(zgb_elem=zgb_list; zgb_elem != NULL; zgb_elem=(zigbee*)(zgb_elem->hh.next)) {
						nb=sprintf((char*)buffer,"%02x:%02x:\n",NEWSENSOR,zgb_elem->key);
						if(send(dialogSocket,buffer,nb,0)==-1){ perror("send"); exit(1);}}}
				
				//add msg to circular-list
				if ((msg_elem = (msg*)malloc(sizeof(msg))) == NULL) exit(-1);
				msg_elem->ident=id;
				msg_elem->sockFd=dialogSocket;
				HASH_ADD_INT(msg_list, ident, msg_elem);	
								
				//---- start a new dialog thread ----
				  pthread_t th;
				  int *arg=(int *)malloc(sizeof(int));
				  *arg=id;
				  if(pthread_create(&th,(pthread_attr_t *)0,send_serial,arg))
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


void *
send_serial(void* arg)
{
	pthread_detach(pthread_self());
	
	//Recover ID
	int Id=*((int*)(arg));
	free(arg);
	int dialogSocket=0;
	
	//Circular list element
	zigbee *zgb_elem=NULL;
	msg *msg_elem=NULL;
	//Trame elements
	unsigned int position,zgb_num,reg_add,reg_num,reg_data;
	char action[3],sensor,r_w;
	action[2]='\0';
	char param_str[25];
	int param_len=0;
	//---------------------
	
	
	//Verification of a Conection
	if(msg_list!=NULL){
		HASH_FIND_INT(msg_list, &Id, msg_elem);
		if(msg_elem!=NULL){
			//Save on local values			
			dialogSocket=msg_elem->sockFd;		
			//Defining Memory space for arguments
			char * buffer=(char *)malloc(FRAME+BUFFSIZE);
			unsigned char * API_frame=(unsigned char *)malloc(HEADER+FRAME+ZBADDR+BUFFSIZE+CHECKSUM);
			
			while(1){
				//Comunnication via tcp
				// ... receive and display message from client
				int nb=recv(dialogSocket,buffer,FRAME+BUFFSIZE-1,0);
				if(nb<=0) { break; }
				buffer[nb]='\0';//Maybe not necesary
				sscanf(buffer,"%02x%c%c%02x",(unsigned int*)&position,action,action+1,(unsigned int*)&zgb_num);	
				//Transmit and RemoteAT
				if((*action)=='T'||(*action)=='R'){
					if(zgb_list!=NULL){
						HASH_FIND_INT(zgb_list, &zgb_num, zgb_elem);//Find Zigbee Address
						if(zgb_elem!=NULL){
							if(buffer[6]=='*'){ //Remote ATcommand Ascii
								sscanf(buffer,"%02x%c%c%02x%c%c*%s",(unsigned int*)&position,action,action+1,(unsigned int*)&zgb_num,(char*)&sensor,(char*)&r_w,param_str);	
								int len=strlen(param_str);	
								for(int i=0;i<len;i++)buffer[3+8+2+3+i]=param_str[i];
								buffer[3+8+2+3+len]=0x00;
								param_len=len;}
							else {
								sscanf(buffer,"%02x%c%c%02x%c%c%02x%02x%02x",(unsigned int*)&position,action,action+1,(unsigned int*)&zgb_num,(char*)&sensor,(char*)&r_w,(unsigned int*)&reg_add,(unsigned int*)&reg_num,(unsigned int*)&reg_data);	
								buffer[3+8+2+4]=reg_add; //Register Address
								buffer[3+8+2+5]=reg_num; //Number of register to read
								buffer[3+8+2+6]=reg_data; //Parameter optional
								param_len=6+((nb-12)/2);} //If no parameter nb=12 and param_len= position+sensor+r_w+reg_add+reg_num
							buffer[0]=Id;
							buffer[1]= *action;
							buffer[2]=*(action+1);
							for(int i=0;i<8;i++)buffer[i+3]=zgb_elem->address[i];
							for(int i=0;i<2;i++)buffer[i+3+8]=zgb_elem->network[i];
							buffer[3+8+2]=Id;
							buffer[3+8+2+1]=position;
							buffer[3+8+2+2]=sensor;
							buffer[3+8+2+3]=r_w;
							}
						else {printf("No Xbee with ID:%d",zgb_num);
								nb=sprintf((char*)buffer,"%02x: No Xbee with ID:%d",ALERT,zgb_num);
								if(send(dialogSocket,buffer,nb,0)==-1){ perror("send"); exit(1); }}}
					else {printf("No Xbee conected yet\n");
							nb=sprintf((char*)buffer,"%02x:No Xbee conected yet\n",ALERT);
							if(send(dialogSocket,buffer,nb,0)==-1){ perror("send"); exit(1);}}}
				//Local AT
				else {
					if(buffer[6]=='*'){//ATcommand
						sscanf(buffer,"%02x%c%c%c%c*%s",(unsigned int*)&position,action,action+1,(char*)&sensor,(char*)&r_w,param_str);
						int len=strlen(param_str);	
						for(int i=0;i<len;i++)buffer[5+i]=param_str[i];
						buffer[5+len]=0x00;
						param_len=len;}
					else {
						sscanf(buffer,"%02x%c%c%c%c%02x%02x",(unsigned int*)&position,action,action+1,(char*)&sensor,(char*)&r_w,(unsigned int*)&reg_add,(unsigned int*)&reg_num);	
						buffer[5]=reg_add; //Parameter 1
						buffer[6]=reg_num; //Parameter 2
						param_len=(nb-6)/2;}
					buffer[0]=Id;
					buffer[1]= *action;
					buffer[2]=*(action+1);
					buffer[3]=sensor; //In this case is not the sensor is the ATcommand
					buffer[4]=r_w;} //In this case is not the sensor is the ATcommand
				//Generate Data_Frame	
				int APIFrame_size = FrameData_gen((unsigned char *)buffer,param_len,API_frame);
				//send question to serial
				//sem_wait (&mutex);
				if(APIFrame_size != (write(serialFd,API_frame,APIFrame_size)))(printf("Fail to send APIframe to serial\n"));
				//sem_post (&mutex);
			}
			//Free Memory
			free(buffer);
			free(API_frame);
		}
	}
	//---- close listen socket ----
	printf("client disconected\n");
	close(msg_elem->sockFd); 
	//Erase element
	HASH_DEL(msg_list, msg_elem);
	return (void *)0;
}



void reponse(unsigned char * buf,int n)
{	
	msg *msg_elem=NULL;
	zigbee *zgb_elem=NULL;
	unsigned int nb, id;
	unsigned char * buffer=(unsigned char *)malloc(FRAME+ZBADDR+BUFFSIZE);
	
	//Copy buf to local
	unsigned char *packet=(unsigned char*)malloc(n);
	for(int i=0;i<n;i++)packet[i]=buf[i];
	packet[n-1]='\0';
	
#if DEBUG	
	printf("Receive: ");
	for(int i=0;i<n;i++){printf("%02x",packet[i]);}
	printf("\n");
	printf("API Identifier:%x\n",packet[3]);
#endif
	
	
	//for(msg_elem=msg_list; msg_elem != NULL; msg_elem=(msg*)(msg_elem->hh.next)) {printf("key %d, sock %d\n", msg_elem->id, msg_elem->sockFd);}
	switch(packet[3]){
		case NODEID:
			// add Zigbee to circular-list
			if((zgb_elem = (zigbee*)malloc(sizeof(zigbee)))==NULL) exit(-1);
			zgb_elem->key=zgb_id++;
			for(int i=0;i<2;i++)zgb_elem->network[i]=packet[15+i];
			for(int i=0;i<8;i++)zgb_elem->address[i]=packet[17+i];
			HASH_ADD_INT(zgb_list, key, zgb_elem);

			//Print the new connection
				printf("new connection-> ID:%d from ",zgb_elem->key);
				for(int i=0;i<8;i++)printf("%02x.",zgb_elem->address[i]);
				printf(":");
				for(int i=0;i<2;i++)printf("%02x",zgb_elem->network[i]);
				printf("\n");
						//Request list of Sensors
						//Generate Data_Frame
						/*
						buffer[0]=0x1;buffer[1]='T';buffer[2]='R';
						for(int i=0;i<8;i++)buffer[i+3]=zgb_elem->address[i];
						for(int i=0;i<2;i++)buffer[i+3+8]=zgb_elem->network[i];
						buffer[3+8+2]='K';
						
						for(int i=0;i<14;i++)printf("%02x",buffer[i]);printf("\n");
						
						int APIFrame_size = FrameData_gen((unsigned char*)buffer,1,API_frame);
						//Write to Serial
						if(APIFrame_size != (write(serialFd,API_frame,APIFrame_size)))(printf("Fail to send APIframe to serial\n"));
						*/
			//Sent new zigbee to Dialog socket
			for(msg_elem=msg_list; msg_elem != NULL; msg_elem=(msg*)(msg_elem->hh.next)) {
				int nb;
				nb=sprintf((char*)buffer,"%02x:%02x:\n",NEWSENSOR,zgb_elem->key);
				if(send(msg_elem->sockFd,buffer,nb,0)==-1){ perror("send"); exit(1); }}

			break;
		
		
		case ZBTRANSTAT://ZigBee Transmit Status
			printf("Message ID: %02x\n",packet[4]);
			if(msg_list!=NULL){
				id=(unsigned int)packet[4];
				HASH_FIND_INT( msg_list, &id, msg_elem );
				if(msg_elem==NULL)printf("Msg:Does not exist");
				else{//Send to Dialogsocket Status
					if(packet[8]!=0x00)nb=sprintf((char*)buffer,"%02x:Error %02x\n",ALERT,packet[8]);
					else nb=sprintf((char*)buffer,"%02x:Success\n",ALERT);
					if(send(msg_elem->sockFd,buffer,nb,0)==-1){ perror("send"); exit(1); }	
				} 
			}
			else printf("List:Any Webpage connected, that's kind of wird\n");
			break;
		case ZBRECVPCK:
			printf("Message ID: %02x\n",packet[15]);
			if(msg_list!=NULL){
				id=(unsigned int)packet[15];
				HASH_FIND_INT( msg_list, &id, msg_elem );
				if(msg_elem==NULL)printf("Msg:Does not exist");
				else{//Format Answer
					//Send to Dialogsocket Answer
					nb=sprintf((char*)buffer,"%02x:Receive %02x %02x\n",packet[16],packet[17],packet[18]);
					if(send(msg_elem->sockFd,buffer,nb,0)==-1){ perror("send"); exit(1); }} 
				}
			else printf("List:Does not exist\n");
			break;
			
		case ATRESPONSE:
			printf("Message ID: %02x\n",packet[4]);
			if(msg_list!=NULL){
				id=(unsigned int)packet[4];
				HASH_FIND_INT( msg_list, &id, msg_elem );
				if(msg_elem==NULL)printf("Msg:Does not exist");
				else{
					char ret[25], aux[3];
					int i=0;
					ret[0]='\0';
					while(packet[8+i]!=0){sprintf(aux,"%02x",packet[8+i]);strcat(ret,aux);i++;}
					nb=sprintf((char*)buffer,"%02x:%c%c->%s Status%s:\n",ATDIV, packet[5],packet[6], ret, (packet[7]==0x00)? "OK":"BAD");
					if(send(msg_elem->sockFd,buffer,nb,0)==-1){ perror("send"); exit(1); }} 	
				}
			break;
			
		case RATRESPONSE:
			
			break;
			//Send to seria
		
	}
	free(packet);
	free(buffer);
#if DEBUG
	printf("fin\n");
#endif
	return;
}

//----------------------------------------------------------------------------
