
#include"mssw.h"

extern int gethostname(char* name,int namelength);		//	Cite statement of gethostname().

extern int kill(pid_t pid,int sig);				//	Cite statement of kill().

extern pid_t wait(int* stat_loc);				//	Cite statement of wait().




static void CHILD(int SIG);			//	Capture signal from father process.

static void CHILD(int SIG)
{
	fprintf(stderr,"Capture from father process,shutdown...\n");
	exit(1);
}


//	Definition of functions : 

short int CGROUP(const char* COMMAND)
{

	if (strncmp(COMMAND,"HELP",4) == 0)	//	Match - HELP
	{
		printf("\nHELP : \n");
		printf("Command : \n");
		///
		printf("\tHELP : Show help information.\n");
		printf("\tEXIT : Close process.\n");
		printf("\tSWITCH : Switch peer.\n");
		///
		return 0;
	}
	else if (strncmp(COMMAND,"EXIT",4) == 0)	//	Match - EXIT
	{
		printf("\nProcess has be exitting...\n");
		return 1;

	}
	else if (strncmp(COMMAND,"SWITCH",6) == 0)	//	Match - SWITCH
	{
		printf("\nSwitching peer - \n");
		return 2;
	}
	else
		return 3;

	//	0	-	HELP
	//	1	-	EXIT
	//	2	-	SWITCH
	//	3	-	Not defined command,probable it is data

}

void VIEW(const struct DATA* data)
{
	//	If buffer is null,do not to view information.
	if (data->MESSAGE[0] == '\0' || data->ADDRESS[0] == '\0')	//	Test
	{
		printf("\nDetected a error in data from peer.\n");
		return;
	}
	else;
	///				Typographic output.
	putchar('\n');
	///
	for (int i=0; i < 10; ++i)
		putchar('*');
	///
	printf("\n\n%s\n",data->MESSAGE);
	printf("From : %s\n\n",data->ADDRESS);
	///
	for (int i=0; i < 10; ++i)
		putchar('*');
	///
	putchar('\n');
	///				Typographic output.

	return;
}

//	Using TCP, CS == 1 as server,CS == 0 as client.

bool CONTROL(const short int CS)
{
	//	Socket arguments.
	int sock;			//	Socket.
	int Length=0;			//	Length of address.
	struct hostent* H_infor;	//	Localhost information.
	struct sockaddr_in address;	//	socket address struct.
	char HOSTNAME[32];		//	Host name.



	//	Temp data buffer.
	struct DATA* TEMP_DATA=(struct DATA*)malloc(sizeof(struct DATA));
	char* COMMAND=(char*)malloc(sizeof(char) * 10);
	if (TEMP_DATA == NULL || COMMAND == NULL)
	{
		perror("Can not get memory for DATA buffer...\n");
		return false;
	}
	else
	{
		memset(TEMP_DATA,'0',sizeof(TEMP_DATA));
		memset(COMMAND,'\0',sizeof(COMMAND));
	}
	//	Write zero to memory.


	//	Set host information.

	if (gethostname(HOSTNAME,32) == 0)	//	zero success.
	{
		H_infor=gethostbyname(HOSTNAME);	//	Get information of host.
		if (H_infor == NULL)			//	NULL is error.
		{
			fprintf(stderr,"Can not get information of this host...\n");
			free(TEMP_DATA);
			free(COMMAND);
			return false;
		}
		else;
		
		address.sin_family=H_infor->h_addrtype;	//	Type for Address.	Normal is AF_INET.
		address.sin_port=htons(NUM);		//	Set port number.Using default.
		
		switch (CS)				//	Set address.
		{
			case 1:
				address.sin_addr.s_addr=inet_addr(H_infor->h_addr_list[0]);
				break;
			case 0:
				puts("Please enter IP-Address of peer,key \'ENTER\' as complete - ");
				fgets(HOSTNAME,32,stdin);	//	Use HOSTNAME as temp-array.
				HOSTNAME[strlen(HOSTNAME)-1]='\0';
				strncpy(TEMP_DATA->MESSAGE,HOSTNAME,ADDR);	//	Copy address.
				address.sin_addr.s_addr=inet_addr(HOSTNAME);	//	Transinformation address string.
				break;
			default:
				fprintf(stderr,"CS = %d have not be defined...\n",CS);	//	error.
				free(TEMP_DATA);
				free(COMMAND);
				return false;
		}

	}
	else;

	///////////////////////////////////////////////////////////////////////
	
	
	//	PID for sub-process.
	
	pid_t PID;			//	sub-process'PID.
	static pid_t child_pid=0;	//	A variable as static.Save PID of child process.


	
	//	Set host information.
	


	switch (CS)
	{
		case 1:	//	Server.
			sock=socket(H_infor->h_addrtype,SOCK_STREAM,0);		//	Create a socket.
			Length=sizeof(address);					//	Size of address.
			if (bind(sock,(const struct sockaddr*)&address,Length) == -1)	//	Bind NSAP.
			{							//	Error.
				fprintf(stderr,"Bind NSAP failed...\n");
				free(TEMP_DATA);
				free(COMMAND);
				return false;
			}
			else
			{							//	Start up server.
				listen(sock,1);
				fprintf(stderr,"Wait connection from client...\n");
				int new_sock;					//	New sock.
				new_sock=accept(sock,(struct sockaddr*)&address,&Length);	//	Wait connection.
				strncpy(TEMP_DATA->ADDRESS,inet_ntoa(address.sin_addr),ADDR);	//	Get peer'address.
				TEMP_DATA->PORT=NUM;						//	Default port number.
			
				short int RESULT=1024;				//	Value from return.
				PID=fork();

				if (PID == 0)					//	Father process.
				{	//	Father process output.

					do
					{
						read(new_sock,TEMP_DATA->MESSAGE,BUFFER);	//	Read data.
						strncpy(COMMAND,TEMP_DATA->MESSAGE,8);		//	Check command.
						RESULT=CGROUP(COMMAND);
					
						switch (RESULT)
						{
							case 1:	//	EXIT.
								kill(child_pid,SIGTERM);		//	Shutdown the child process.Send SIGTERM.
								break;
							case 2: //	SWITCH.			//	SWITCH command is from peer.
//								RESULT=1;
//								fprintf(stderr,"User has quitted...\n");
//								kill(child_pid,SIGTERM);		//	Shutdown the child process.Send SIGTERM.
//								break;
//								
//			Dosen't use SWITCH command as temporarily.

								fprintf(stderr,"SWITCH command could not has be usage.\n");

							case 0:	//	HELP.
								break;
							case 3:	//	MESSAGE.
								VIEW(TEMP_DATA);
								break;
							default:
								break;
						}

					} while (RESULT == 1);

					//	Recycle memorys.
					free(TEMP_DATA);
					free(COMMAND);

					//	Front of recycle,father process must send signal to child process.Usage kill() at switch() on over.


					int stat_val;
					PID=wait(&stat_val);			//	Wait child process was finished...
					if (WIFEXITED(stat_val) != 0)
						fprintf(stderr,"Child process was finished,so process exitting...\n");
					else
						fprintf(stderr,"Error,detected a error when the child process is exitting..\n");
					
					//	Shutdown socket from accept().
					if (close(new_sock) == 0)
						fprintf(stderr,"As Server,Shutdown new socket success.\n");
					else
						fprintf(stderr,"As Server,Shutdown new socket failed.\n");
					
				}
				else						//	Child process.
				{
					//	Child process input.
					//	Child process must use new temp-buffer.Because in father process the buffer is from malloc().
					int Len=1024;				//	Length of message.
					struct DATA TEMP;			//	Temp data buffer,in child process space.
					char command[10];			//	Temp command buffer,in child process space.
					//	buffer.

					memset(&TEMP,'0',sizeof(TEMP));
					memset(command,'\0',sizeof(command));\
					child_pid=getpid();			//	Get child process PID.
					//	Change hevior with captured a signal.

					signal(SIGTERM,CHILD);			//	SIGNAL hevior.

					puts("Now,you can enter messages - \n");

					do
					{
						fgets(TEMP.MESSAGE,BUFFER,stdin);
						Len=strlen(TEMP.MESSAGE)-1;	//	Delete \n.
						TEMP.MESSAGE[Len]='\0';		//	Delete \n.
						strncpy(command,TEMP.MESSAGE,8);		//	Check command.
						RESULT=CGROUP(command);
						
						switch(RESULT)
						{
							case 1:	//	EXIT.
								//	Exitting.
								fprintf(stderr,"Server shutdown...\n");
								//	Exit this child process.
								break;

							case 2:	//	SWITCH.			//	But server is not response for SWITCH COMMAND.
							case 0:	//	HELP.
								break;

							case 3:	//	MESSAGE.
								write(new_sock,TEMP.MESSAGE,Len);	//	Send message.
								break;
							default:
								break;
						}

					} while (RESULT == 1);
					
					exit(1);		//	Child process quit.
//
					//	When child process has been quitted,local variable has be recycle.
				}
			}
			break;
		case 0:	//	Client.
			sock=socket(H_infor->h_addrtype,SOCK_STREAM,0);				//	Create a socket as client.
			if (sock == -1)
			{
				perror("Create socket failed...\n");
				return false;
			}
			else
				fprintf(stderr,"Connecte to server...\n");
			Length=sizeof(address);		//	Size of address.
			
			if (connect(sock,(const struct sockaddr*)&address,Length) == 0)		//	Connection to server.
			{
				TEMP_DATA->PORT=NUM;						//	Default port number.
						
				short int RESULT=1024;				//	Value from return.
				PID=fork();

				if (PID == 0)					//	Father process.
				{	//	Father process output.

					do
					{
						read(sock,TEMP_DATA->MESSAGE,BUFFER);	//	Read data.
						strncpy(COMMAND,TEMP_DATA->MESSAGE,8);		//	Check command.
						RESULT=CGROUP(COMMAND);
					
						switch (RESULT)
						{
							case 1:	//	EXIT.
								kill(child_pid,SIGTERM);		//	Shutdown the child process.Send SIGTERM.
								break;
//							case 2: //	SWITCH.			//	SWITCH command is from peer.
//								RESULT=1;
///								fprintf(stderr,"User has quitted...\n");
//								kill(child_pid,SIGTERM);		//	Shutdown the child process.Send SIGTERM.
//								break;
							case 0:	//	HELP.
								break;
							case 3:	//	MESSAGE.
								VIEW(TEMP_DATA);
								break;
							default:
								break;
						}

					} while (RESULT == 1);

					//	Recycle memorys.
					free(TEMP_DATA);
					free(COMMAND);

					//	Front of recycle,father process must send signal to child process.Usage kill() at switch() on over.

					int stat_val;				//	State of child process.

					PID=wait(&stat_val);			//	Wait child process was finished...
					if (WIFEXITED(stat_val) != 0)
						fprintf(stderr,"Child process was finished,so process exitting...\n");
					else
						fprintf(stderr,"Error,detected a error when the child process is exitting..\n");

					//	Father process code.					

				}
				else						//	Child process.
				{
					//	Child process input.
					//	Child process must use new temp-buffer.Because in father process the buffer is from malloc().
					int Len=1024;				//	Length of message.
					struct DATA TEMP;			//	Temp data buffer,in child process space.
					char command[10];				//	Temp command buffer,in child process space.
					//	buffer.

					memset(&TEMP,'0',sizeof(TEMP));
					memset(command,'\0',sizeof(command));\
					child_pid=getpid();			//	Get child process PID.
					//	Change hevior with captured a signal.

					signal(SIGTERM,CHILD);			//	SIGNAL hevior.

					puts("Now,you can enter messages - \n");

					do
					{
						fgets(TEMP.MESSAGE,BUFFER,stdin);
						Len=strlen(TEMP.MESSAGE)-1;	//	Delete \n.
						TEMP.MESSAGE[Len]='\0';		//	Delete \n.
						strncpy(command,TEMP.MESSAGE,8);//	Check command.
						RESULT=CGROUP(command);
						
						switch(RESULT)
						{
							case 1:	//	EXIT.
								//	Exitting.
								fprintf(stderr,"Client shutdown...\n");
								write(sock,"EXIT",4);
								//	Exit this child process.
								break;

							case 2:	//	SWITCH.	In client.
												//	Dosen't use SWITCH as temporarily.
//								write(sock,"SWITCH",6);
//								RESULT=1;
//								break;

								fprintf(stderr,"SWITCH command could not has be usage.\n");		
	
							case 0:	//	HELP.
								break;

							case 3:	//	MESSAGE.
								write(sock,TEMP.MESSAGE,Len);	//	Send message.
								break;
							default:
								break;
						}

					} while (RESULT == 1);
					
					exit(1);		//	Child process quit.
//
					//	When child process has been quitted,local variable has be recycle.
				}
			}
			else
			{
				fprintf(stderr,"Can not connection to server...\n");
				free(TEMP_DATA);
				free(COMMAND);							//	Recycle.
				return false;
			}


			break;
		default:
			//	Have not be defined.
			free(TEMP_DATA);
			free(COMMAND);
			fprintf(stderr,"CS = %d have not be defined...\n",CS);
			return false;
	}


	if (close(sock) == 0)
	{
		fprintf(stderr,"Process exitting...\n");
	}
	else
	{
		fprintf(stderr,"Process had detected a error...\n");
		return false;
	}


	return true;

}






























