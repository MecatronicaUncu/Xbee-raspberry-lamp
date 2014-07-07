#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include "APIframe_gen.h"



#define TEST 0

#if TEST
/* Test example ...*/
int main(int argc, char* argv[]){
	unsigned char buffer[0x100];
	printf("Usage: %s FrameID cmID [Specific Data...]\n",argv[0]);

}
#endif

/*
 *  FrameData_gen
 *  
 *
 *  Created by Alonso & Badaloni on 04/04/13.
 *  Copyright 2013 ENIB. All rights reserved.
 *
 */
size_t FrameData_gen(unsigned char *buffer,int param_len,unsigned char *API_frame){

	//Number of bytes on the table
	int num_bytes=0;
	//---- Frame-specific Data / cmdData ----
	//---- Frame Type ----
	char to_str[3];
	to_str[0]=buffer[1];
	to_str[1]=buffer[2];
	to_str[2]='\0';
	if (!strcmp(to_str,"AT")) {//AT Command
		API_frame[HEADER]=ATCMD;}
	else if (!strcmp(to_str,"RT")) {//AT Command-Remote Request
		API_frame[HEADER]=RATCMD;}
	else if (!strcmp(to_str,"TR")) {//ZigBee Transmit Request
		API_frame[HEADER]=ZBTR;}
	else {fprintf(stderr,"No API msg: %s",to_str);return 0;}
	//---- Frame ID / Identifier ----
	API_frame[HEADER+1]=buffer[0];
	//---- Specific Data ----
	switch (API_frame[HEADER]) {
		case ATCMD:
		//AT Command
			API_frame[HEADER+FRAME]=buffer[3];
			API_frame[HEADER+FRAME+1]=buffer[4];
			//Parameter Value
			for(int i=0; i<param_len;i++)API_frame[HEADER+FRAME+ATCM+i]=buffer[5+i];
			num_bytes=FRAME+ATCM+param_len;
			break;
			
		case RATCMD:
		//AT Command-Remote Request
			//[Destination Address + Address network ] 
			for(int i=0; i< RATCM;i++)API_frame[HEADER+FRAME+i]=buffer[3+i];
			//Remote Command Options
			API_frame[HEADER+FRAME+RATCM]=COMOPTION;
			//AT Command
			API_frame[HEADER+FRAME+RATCM+1]= buffer[5+RATCM];
			API_frame[HEADER+FRAME+RATCM+2]= buffer[6+RATCM];
			//Command Parameter
			for(int i=0; i<param_len;i++)API_frame[HEADER+FRAME+RATCM+2+ATCM+i]=buffer[i+7+RATCM];
			num_bytes=FRAME+RATCM+2+ATCM+param_len;
			break;
	
		case ZBTR:
		//ZigBee Transmit Request
			//[Destination Address + Address network ] 
			for(int i=0; i< ZBADDR;i++)API_frame[HEADER+FRAME+i]=buffer[i+3];
			//Broadcast Radius + Options
			API_frame[HEADER+FRAME+ZBADDR]=BROADCAST_RADIUS;
			API_frame[HEADER+FRAME+ZBADDR+1]=OPTIONS;
			//RF DATA
			for(int i=0; i<param_len;i++)API_frame[HEADER+FRAME+ZBADDR+2+i]=buffer[i+3+ZBADDR];
			num_bytes=FRAME+ZBADDR+2+param_len;
			break;
	}
	//----Frame-Specific Data----Finished impression
	printf("Frame-Specific Data: ");
	for (int i=0; i<num_bytes; i++) {printf("%02x",API_frame[i+3]);}
	printf("\n");
	
	//----API Frame----Start
	API_frame[0] = 0x7E; //Star Delimiter
	//Lenght
	API_frame[1] = ((num_bytes)>>8)&0xFF;//Lenght MSB
	API_frame[2] = (num_bytes) & 0xFF;//Length LSB
	//Checksum
	unsigned int checksum=0; 
	for (int i=0; i<num_bytes ; i++) {checksum+=API_frame[i+3];}
	API_frame[HEADER+num_bytes]= (0xFF)-(checksum & 0xFF);
	//----API Frame----Finished impression
	printf("Frame API: ");
	for(int i=0;i<HEADER+num_bytes+CHECKSUM;i++){printf("%02x",API_frame[i]);}
	printf("\n");

	return HEADER+num_bytes+CHECKSUM;
}
