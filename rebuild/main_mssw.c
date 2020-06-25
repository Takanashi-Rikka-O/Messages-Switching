#include"mssw.h"

//extern int waitpid(pid_t pid,int* stat_loc,int options);

//	Stop child process.
static void INTERRUPT(pid_t PID);


int main(void)
{
	//	PID variable.
	pid_t PID;
	//	Socket IO
	int socket_IO=-1;
	//	Initialize this socket_IO.Because gcc will view warring when used with uninitialized.

	//	Statement a variable for save return values.
	short int result=1024;

	//	Statement a pointer for structure hostent.
	struct hostent* host_info;

	//	Statement a array to save hostname.
	char* HOSTNAME=(char*)calloc(36,sizeof(char));
	if (HOSTNAME)
		result=gethostname(HOSTNAME,36);	//	Get host name.

	else
	{
		//	Show error message.
		fprintf(stderr,"Can not get memrory for HOSTNAME.\n");
		exit(1);
	}

	//	Check return.
	switch (result)
	{
		case 0:		//	Successed.
			//	Get hostent info.
			host_info=gethostbyname(HOSTNAME);
			if (host_info)
			{
				//	Recycle memory.
				free(HOSTNAME);

				//	Creat socket.
				socket_IO=socket(host_info->h_addrtype,SOCK_DGRAM,0);
				if (socket_IO > 0)

					//	Debug check value of socket.
					#ifdef DEBUG
						fprintf(stderr,"Created a UDP socket %d - \n",socket_IO);
					#else
						;
					#endif
				else
				{
					//	Show error message.
					fprintf(stderr,"Can not creat socket.\n");
					exit(4);
				}
				break;
			}
			else
			{
				//	Show error message.
				fprintf(stderr,"Can not get host info.\n");
				//	Recycle resources.
				free(HOSTNAME);
				close(socket_IO);
				exit(3);
			}
		default:	//	Failed.
			//	Show error message.
			fprintf(stderr,"Can not get hostname.\n");
			//	Close socket.
			close(socket_IO);
			exit(2);
	}




	//	Start child process up.
	PID=fork();

	//	Initialization.
	result=INITIALIZER(PID,host_info,socket_IO);

	switch (PID)
	{
		case 0:		//	Child process.
			//	Check status of initialization.
			switch (result)
			{
				case -2:
					fprintf(stderr,"Child-process : Signal action invalid.\n");
					exit(-2);
				case -22:
					fprintf(stderr,"Child-process : bind socket failed,\n");
					exit(-22);
				default:
					//	Start listen work.
					result=OUTPUT(socket_IO);

					//	Check return
					switch (result)
					{
						case -1:
							fprintf(stderr,"Child-process : Can not get memmory for output buffer.\n");
						exit(-1);
						default:
							fprintf(stderr,"\nChild-process : Shutdown talk...\n");
					}				
			}

			break;
	
		default:	//	Parent process.
			//	Check status of intialization.
			switch (result)
			{
				case -1:	//	Set signal action failed.
					fprintf(stderr,"Parent-process : Signal action invalid.\n");
					//	Shutdown child process.
					INTERRUPT(PID);
					break;
				case -7:	//	IPC creat failed.
					fprintf(stderr,"Parent-process : Creat IPC Object was failed.\n");
					//	Shutdown child process.
					INTERRUPT(PID);
					break;
				case -8:	//	IPC semctl failed.
					fprintf(stderr,"Parent-process : Initialize IPC Object was failed.\n");
					//	Shutdown child process.
					INTERRUPT(PID);
					union semun tmp1;
					//	Delete IPC Object
					(void)semctl(IPC_ID,0,IPC_RMID,tmp1);
					break;

				default:
					//	Normal
					//	Start talk work.
					//result=INPUT(socket_IO,PID);

					result=INPUT(socket_IO);
					//	Check status for INPUT.
					switch (result)
					{
						case -1:	//	Error.
							fprintf(stderr,"Parent-process : Can not get memory for input buffer.\n");
							//	Shutdown child process.
							INTERRUPT(PID);
							//	Delete IPC Object
							union semun tmp2;
							(void)semctl(IPC_ID,0,IPC_RMID,tmp2);
							break;

						//	When child process stopped,it will send SIGCHLD to parent-process.So here not have to do some.
						//	Recycle work is transfer to label 'default'.
						/*
						case -9:
							fprintf(stderr,"Parent-process : Child process just had been died,\n");
							union semun tmp3;
							(void)semctl(IPC_ID,0,IPC_RMID,tmp3);
							break;
						*/
						default:	//	Success
							fprintf(stderr,"\nParent-process : Shutdown talk...\n");
							//	Shutdown child process.
							INTERRUPT(PID);
							//	Delete IPC.
							union semun tmp4;
							(void)semctl(IPC_ID,0,IPC_RMID,tmp4);
							
							//	Close socket.
							close(socket_IO);
						
					}
			}


	}


	exit(0);
}

static void INTERRUPT(pid_t PID)
{
	//	Send SIGINT
	#ifdef DEBUG
		fprintf(stderr,"INTERRUPT kill return %d .\n",kill(PID,SIGINT));
	#else
		(void)kill(PID,SIGINT);
	#endif
	//	Wait child process has been quitted.
	#ifdef DEBUG
		fprintf(stderr,"Wait child process.\n");
	#endif
	waitpid(PID,NULL,0);
	//	Return
	return;
}

