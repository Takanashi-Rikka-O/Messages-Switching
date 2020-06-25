
#ifndef _MSSW_H_
#define _MSSW_H_

//	Open X/OPEN standard.

#define _XOPEN_SOURCE


//	UNIX

#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/sem.h>
#include<errno.h>


//	NET

#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//	C

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//	Define port
#define DFPORT 56636


//	User define union semun
union semun{
	int val;
};


//	Global variables

extern short int CONDITION;
extern int IPC_ID;

//	Functions

int INITIALIZER(pid_t PID,struct hostent* HOST_INFO,int SOCKET);
/*
 *	0 - Succeed.
 *
 *	-1 - signal handler set failed in parent process.
 *	-7 - IPC creat failed in parent process.
 *	-8 - semctl failed.
 *
 *	-2 - signal handler set failed in child process.
 *	-22 - child process bind socket failed.
*/ 
//	Receive and Send will be using same socket.



//int INPUT(int i_socket,pid_t PID_C);
int INPUT(int i_socket);
/*
 *	0 - Success.
 *	-1 - Get memory failed.
*/ 

int OUTPUT(int o_socket);
/*
 *	0 - Success.
 *	-1 - Get memory failed.
*/ 


//	Extern functions

extern int gethostname(char* name,int namelength);
extern int kill(pid_t pid,int sig);
extern pid_t waitpid(pid_t pid,int* stat_loc,int options);



#endif

