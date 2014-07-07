#if !defined(_APIFRAME_GEN_H)
#define _APIFRAME_GEN_H

#define ATCMD			0X08	//AT Command
#define RATCMD			0x17	//AT Command-Remote Request
#define	ZBTR			0x10	//ZigBee Transmit Request

#define HEADER	        3		//Size of Star delimiter + LengthMSB + LengthLSB
#define	FRAME    		2  		//Size of Frame Type + Frame ID (bytes)
#define ATCM       		2 		//Size of ATcommand(bytes) 
#define ZBADDR 			10		//8 bytes destination address + 2 bytes destinetion network address  
#define RATCM			10		//8 bytes destination address + 2 bytes destinetion network address  
#define BUFFSIZE		72
#define CHECKSUM		1   //1 byte for checksum


#define COMOPTION	0x02 		/*Remote Command Options:
								*0x01:Disable retries and route repair
								*0x02:Apply changes
								*0x20:Enable Aps encryption
								*0x40:Extended transmition timeouts
								*/  
#define BROADCAST_RADIUS 0x00  /*Set the maximun numbers of hups a broadcast transmition can traverse
								*If 0 Tx radius set to the network maximun hups (10)
								*/
								
#define OPTIONS			0x00	/*0x01 Disable ACK
								*0x02 Disable Network Address Discovery
								*/ 

//size_t FrameData_gen(unsigned char *buffer,unsigned char *API_frame);
size_t FrameData_gen(unsigned char *buffer,int param_len,unsigned char *API_frame);

#endif
