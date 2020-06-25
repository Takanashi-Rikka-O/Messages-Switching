///////////////////////////////////////

#ifndef _MSSW_H_
#define _MSSW_H_

//////////////////////////

#include<unistd.h>		//	UNIX library.

#include<sys/types.h>		//	Data type.

#include<signal.h>		//	SIGNAL.

#include<sys/wait.h>		//	wait().

//#include<sys/time.h>		//	Wait state arrvied for File-Script.	
				//	But read function could auto block with default mode.

//////////////////////////

#include<sys/socket.h>		//	Sock.

#include<netdb.h>		//	Net data base.

#include<netinet/in.h>		//	Byte-Order transformat.

#include<arpa/inet.h>		//	Host informat.

//////////////////////////

#include<stdio.h>		//	Standard library of C.
#include<stdlib.h>		//	Standard library of C.
#include<stdbool.h>		//	BOOL type.
#include<string.h>		//	String operation.

//////////////////////////

//	Struct : 

#define BUFFER 8193
#define ADDR 33
#define NUM 9292		//	As default port number.

struct DATA{

	char MESSAGE[BUFFER];	//	MESSAGES buffer.
	char ADDRESS[ADDR];	//	ADDRESS buffer.	
	short int PORT;		//	Port number.

};

/*
 *	View : 
 *
 *---------------------------
 *
 * 	MESSAGES
 *
 *---------------------------
 *	IP-ADDRESS
 *---------------------------
*/ 

//	Function prototype : 


short int CGROUP(const char* COMMAND);		//	Process Control.

void VIEW(const struct DATA* data);		//	Typographic output.

bool CONTROL(const short int CS);		//	Process Control.

//void LOGGER(const char* Log_Message);		//	Process log_messages record.

//////////////////////////

#endif

















