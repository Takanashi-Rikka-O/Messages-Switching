
#include"mssw.h"

//	Global variable.
short int CONDITION=1;
//	This variable does only be needed by parent process.
int IPC_ID;

//	Signal action function
//	SIGINT handlers are not as same with parent process and child process.

//	Parent SIGINT handler.
static void SIG_INT_FUNC_F(int sig);

//	Child SIGINT handler.
static void SIG_INT_FUNC_C(int sig);

//	Synchrounize alarm.Parent process will wait this signal from child process and then continue.
static void SIG_ALRM(int sig);

//	Child' SIGCHLD handler.
static void SIG_CHLD(int sig);

//	Carry port function
//	Carry port on from string of user input - "233.23.4.55:6644"
static unsigned short int Carry_Port(char* IP_PORT);






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


int INITIALIZER(pid_t PID,struct hostent* HOST_INFO,int SOCKET)
{

	//	Determine parent and child.
	switch (PID)
	{
		case 0:		//	In child process
			{
				//	Attention : a label can only be part of a statement,and a declaration is not a statement.
				//
				//	So here must use {} as one piece of code.
				//	If not write {} then has - case 0: struct sigaction act={..};
				//	That is not right,label 'case' must as a part of a statement.
				//	But the struct sigaction act={..}; is not a statement,it is a declaration.
				//	And {} can make the variables at {} become to local variables,does only be see in this scope.
				//	Otherwise 'case other: / default: ' can see them and use them.


				//	Set signalaction.
				
				//	Set signal action,but parent process and child process will use not as same functions.
				struct sigaction act={
						.sa_handler=SIG_INT_FUNC_C,
						.sa_flags=0,
					};
				//	Set mask-set of signal to NULL.
				sigemptyset(&act.sa_mask);
	
				//	Set action for signals.
				if (sigaction(SIGINT,&act,NULL) == 0)
					;
				else
					return -2;

				//	Save return value from functions.
				int result=1024;

				//	Statement a sockaddr_in structure and initialize.

				struct sockaddr_in IP_LOCAL={
						.sin_family=HOST_INFO->h_addrtype,
						};

				//	Hint message.
				fprintf(stderr,"Please select the port of host : ('0' will use default port) : ");			

				do
				{
					//	hu -> unsigned short int
					scanf("%hu",&IP_LOCAL.sin_port);
					//	Check port,it must greater than 1024.
					if (IP_LOCAL.sin_port > 1024)
						break;
					else if (IP_LOCAL.sin_port == 0)
					{
						IP_LOCAL.sin_port=DFPORT;
						break;
					}
					else
						fprintf(stderr,"Error port,it must greater than 1024 and less than 65535!\n");
				} while(1);



				//	In mode DEBUG,process will use localhost at IP-Address.				
				//	Debug in the mode DEBUG,process will use loop net as test NET.
				#ifndef DEBUG
					IP_LOCAL.sin_addr=*((struct in_addr*)*(HOST_INFO->h_addr_list));
				#else
					//	unsigned short int inet_addr(void* str);
					IP_LOCAL.sin_addr.s_addr=inet_addr("127.0.0.1");
				#endif

				//	Had been finished port select,send SIGALRM to wake parent process up.
				(void)kill(getppid(),SIGALRM);

				//	Use bind()to bind socket.
				result=bind(SOCKET,(struct sockaddr*)&IP_LOCAL,sizeof(IP_LOCAL));
				//	Check bind status.
				switch (result)
				{
					case 0:
						//	Hint message for success.
						fprintf(stderr,"Bind socket succeed for child process.\n");

						//	Debug check IP-ADDRESS and PORT with type is net bytes.
						#ifdef DEBUG
							fprintf(stderr,"IP-ADDRESS value %hu,PORT %hu.\n",IP_LOCAL.sin_addr.s_addr,IP_LOCAL.sin_port);
						#endif
						
						return 0;
					default:
						//	Hint message for failure.
						fprintf(stderr,"Bind socket failed for child process.\n");
						//	Print error message for variable-errno.
						strerror(errno);

						return -22;
				}
			}

			break;

		default://	In parent process	
			//	'default' is also a label,so it must as part of statement.
			{
				//	Set signalaction.
				//
				//	Set signal action,but parent process and child process will use not as same functions.
				struct sigaction act1={
						.sa_handler=SIG_INT_FUNC_F,
						.sa_flags=0,
					};
				sigemptyset(&act1.sa_mask);
		
				if (sigaction(SIGINT,&act1,NULL) == 0)
				{
					//	Set SIGALRM,parent process wait this signal and synchronize with child process.
					act1.sa_handler=SIG_ALRM;
					//	Can not set action to SIG_IGN.If set with that when process get signal SIGALRM,the function
					//	int sigsuspend(const sigset_t* sigmask); does not retur
					if (sigaction(SIGALRM,&act1,NULL) == 0)
					{
						act1.sa_handler=SIG_CHLD;	//	SIGCHLD is ignore by parent process with default action.
						//	Change the action when process has got a SIGCHLD.
						if (sigaction(SIGCHLD,&act1,NULL) == 0)
							;
						else
							return -1;
						

					}
					else
						return -1;
				}
				else
					return -1;

				//	Set IPC object.
				//	Creat IPC.
				IPC_ID=semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL);
				//	Check IPC status
				switch (IPC_ID)
				{
					case -1:	//	Error.
						return -7;
					default:	//	Normal.
						{
							//	Initialize IPC.
							//		Initial value = 1.
							union semun temp_union={
										.val=1,	
									};
							//	Initialize that IPC object.
							if (semctl(IPC_ID,0,SETVAL,temp_union) == 0)
								;
							else
								return -8;
						}
				}
		
				//	Before return,parent process must wait SIGALRM from child process.
				sigsuspend(&act1.sa_mask);

				//	When child process had been over with start work,it will send SIGALRM to parent process.
				//	Success
				return 0;
			}

	}

}

/*
 *	-1 - Get memory for buffer was failed.
 *	0 - succeed to quit this function.
*/ 

//	Operation-P - sem-1
//	Operation-V - sem+1

//int INPUT(int i_socket,pid_t PID_C)	//	Use SIGCHLD capturer,parent process not have to check status for child process.
int INPUT(int i_socket)
{

	//	Function get input and send to peer.
	//	Use sendto() to send data.
	//	size_t sendto(int sockfd,const void* buff,size_t nbytes,int flags,struct sockaddr* to,socklen_t address);

	short int BUF_LEN=1024;
	char* INPUT_BUF=(char*)calloc(BUF_LEN,sizeof(char));
	if (INPUT_BUF)
	{
		//	Save return values.
		short int result=BUF_LEN;
		struct sockaddr_in PEER={
				.sin_family=AF_INET,
				};
		size_t LEN_ADDR;

		//	Creat a strucature sembuf.
		struct sembuf sem_ops={
					.sem_num=0,
					.sem_flg=SEM_UNDO,
				};

		//	Statement stat_loc
		//int stat_loc;

		do
		{
			//	Hint.
			fprintf(stderr,"Please enter IP:PORT(Only IP will use default port) for peer (EXIT#) will quit process.\n");
			//	Get data from input.
			fgets(INPUT_BUF,BUF_LEN,stdin);
			//	Delete \n
			INPUT_BUF[strlen(INPUT_BUF)-1]='\0';
		
			//	Debug check string from input.	
			#ifdef DEBUG
				fprintf(stderr,"Check point - INPUT_BUF %s \n",INPUT_BUF);
			#endif


			//	Test - status = have not anyerrors on here.
			//	fprintf(stderr,"Input IP = %s\n",INPUT_BUF);
			//	Check input.
			result=strncmp(INPUT_BUF,"EXIT#",5);	//	If the command from input it is 'EXIT#',process will stop.

			switch (result)
			{
				case 0:	//	Command EXIT#

					//	Check input.
					#ifdef DEBUG
						fprintf(stderr,"Get command EXIT#\n");
					#endif



					//	Here must use IPC to synchronize.
					//	!!!!!!!!!!!!!!!!!!!!!!!
			
					//	int semop(int sem_id,struct sembuf* sem_ops,size_t num_sem_ops);
					/*
 					 * 	sem_id - IPC ID ;
 					 *	sem_ops - Operation set pointer,can use array to do a lot of actions ;
 					 *	num_sem_ops - Numbers of operation set (struct sembuf),it is not size of structure sembuf.
 					*/ 

					//	Excuting operation P.
					//	Set value for P.
					sem_ops.sem_op=-1;

					//	Debug IPC Operation hint.
					#ifdef DEBUG
						fprintf(stderr,"Parent-process : IPC P ID= %d .\n",IPC_ID);
					#endif

					//	P
					(void)semop(IPC_ID,&sem_ops,1);
					//	Change CONDITION after get IPC object.
					CONDITION=0;
					//	Excuting operation V.
					sem_ops.sem_op=+1;

					//	Debug IPC Operation hint.
					#ifdef DEBUG
						fprintf(stderr,"Parent-process : IPC V ID= %d .\n",IPC_ID);
					#endif

					//	V
					(void)semop(IPC_ID,&sem_ops,1);

					//	Debug show value of CONDITION.
					#ifdef DEBUG
						fprintf(stderr,"Just break switch().CONDITION = %d .\n",CONDITION);
					#endif

					break;

				default://	Data

					//	Check IP:PORT string.
					PEER.sin_port=Carry_Port(INPUT_BUF);

					//	Transinformat for ip address string.
					//	int inet_pton(int family,const void* strptr,void* addrptr);
					result=inet_pton(AF_INET,INPUT_BUF,&PEER.sin_addr.s_addr);
					//	Check result
					switch (result)
					{
						case 0:	//	Invalid string.
							fprintf(stderr,"Invalid IP Address...\n");
							//	Try again.
							break;

						case -1:	//	A error has been detected by inet_pton().
							fprintf(stderr,"Detected a error when transinformating for IP Address.\n");
							//	Try again.
							break;
					
						default:	//	Success.
							//	Entry second cycle.
							//	Get length for PEER.
							LEN_ADDR=sizeof(PEER);
							
							//	Clear elements of share array INPUT_BUF.
							//	It proable have IP string in memory.
							memset(INPUT_BUF,'\0',BUF_LEN);
							//	Function void* memset(void* s,int c,size_t n);
							//	Fills the first n bytes of memory,it area pointed to by s with the constant c.
			
							
							//	Show message to hint user may to start talk up.
							fprintf(stderr,"Let we talk with peer!!!(SWITCH# to switch peer)\n");
					
							//	Debug check IP-ADDRESS and Service-Port.
							#ifdef DEBUG
								fprintf(stdout,"IP-ADDRESS from input %hu,PORT %hu\n",PEER.sin_addr.s_addr,PEER.sin_port);
							#endif



							//	Second cycle.
							do
							{
							/*	Parent process will get signal 'SIGCHLD' when child process has been stopped.
								//	Check status for child process.
								if (waitpid(PID_C,&stat_loc,WNOHANG) == 0)
								{
									//	Check if is stopped.
									if (WIFSTOPPED(stat_loc) == 0)
										;
									else
									{
										//	Recycle resource and quit.
										free(INPUT_BUF);
										return -9;
									}

								}
								else
								{
									//	Recycle resource and quit.
									free(INPUT_BUF);
									return -9;
								}
							*/


								//	Read data.
								fgets(INPUT_BUF,BUF_LEN,stdin);

								#ifdef DEBUG
									fprintf(stderr,"Check point - INPUT_BUF %s \n",INPUT_BUF);
								#endif

								//	Check message.
								if (strncmp(INPUT_BUF,"SWITCH#",7) == 0)
									break;
								else
								{
									//	Debug check string from input.
									#ifdef DEBUG
										fprintf(stderr,"Echo : %s \n",INPUT_BUF);
									#endif

									//	Send data to peer.
									result=sendto(i_socket,INPUT_BUF,strlen(INPUT_BUF)-1,0,(struct sockaddr*)&PEER,LEN_ADDR);
									//	Check value from return by sendto
									switch (result)
									{
										case  -1:	//	sendto error.

											//	print error message
											fprintf(stderr,"Detected a error when sending data to peer.\n");
											break;

										default:	//	Do nothing at there.
											//	Debug check value from return by sendto().
											#ifdef DEBUG
												fprintf(stderr,"Value from return by sendto() is %d\n",result);
											#endif
											;
									}

								}
							//	End of Second cycle.
							} while (CONDITION);

					}
			}

		//	End of First cycle.
		} while (CONDITION);

		//	Debug hint message.
		#ifdef DEBUG
			fprintf(stderr,"INPUT() stopped.\n");
		#endif

		//	Recycle memory of INPUT BUFFER.
		free(INPUT_BUF);
		//	Return.
		return 0;
	}	
	else
		return -1;
}



//	Read data from peer and view them.
//	Error : 
//		0 - Succeed.
//		-1 - Can not get memory for buffer.

//	Function of child process,it does only to receive data and view them.
//	When child process has stopped it will send 'SIGCHLD' to parent process.
int OUTPUT(int o_socket)
{

	//	Function for view messages from peer.
	//	Use recvfrom to get data and record address of peer to memory.
	//	size_t recvfrom(int sockfd,const void* buff,size_t nbytes,int flags,const struct sockaddr* from,socklen_t* addrlen);
	
	//	Save values from return by functions.
	short int result=1024;
	//	Length of buffer.
	size_t BUF_LEN=result;
	//	Get memory for buffer.
	char* OUTPUT_BUF=(char*)calloc(BUF_LEN,sizeof(char));	//	size=1024 bytes.

	//	Set this variable to as test numbers for select().
	//	You also to use the default value for fd_set size,it max= 1024 or 31.
	const short int fd_setsize=o_socket+1;
	//	Function select(),when it has been started test up first use fd '0' to test.And then increasing it,"0,1,2,3...".
	//	ndfs prameter is limit for the test sequence.If you have a fd value=6,so must set ndfs=6+1.
	//	Why add one for fd '6'? Because select() will stop at the maxnumber ndfs-1.
	//	Remember ndfs is not the numbers of have to testing fds,it it the limit of that test sequence.
	//	If select() test '6' had been ready to be readed (check '6' then found that '6' had been set in the read_set_region),
	//	it will return 1 for the numbers of these ready fds.

	//	Succeed to get memory for buffer.
	if (OUTPUT_BUF)
	{
		//	This structure will be using to save address of peer by recvfrom().
		struct sockaddr_in ADDR_PEER;
		size_t LEN_ADDR=sizeof(struct sockaddr_in);

		//	Recvfrom() must has be using after the select().
		//	Define timer for select().Time out is 10s.
		const struct timeval TIMER_BAK={
				.tv_sec=10,
				.tv_usec=0,
				};
		//	Why need the back version for timer?
		//	Because if select() has be return by time out,timeval structure will be setting to time left.
		struct timeval TIMER;
		
		//	Fix.If set struct timeval with type const,and then when select() returned it will be not changing value to the time left.
		//	It's too bad,const struct timeval* timeout does only be supported in UNIX.

		//	Define a set for file descript.Read set.
		fd_set r_set;

		//	Initialize this set to zero.
		FD_ZERO(&r_set);


		do
		{
			//	Reset at the head of cycle.
			//	|			|
			//	Adding o_socket to this set.
			//	Because FD_SET will be resetting when select() returned once.Open bit for that socket descript o_socker.
			FD_SET(o_socket,&r_set);
			//	Set TIMER.
			TIMER=TIMER_BAK;


			//	Wait select() return.
			result=select(fd_setsize,&r_set,NULL,NULL,&TIMER);
			//	Remember if select() return by time out,the fd_set will be setting to zero.
			//	Work module of select().It is first to check the '0' file-descript,and increas value until it is equal 'ndfs-1'.
			//	The work that check for file-descripts is responed by kernel.
			//	select() will return the numbers of had be changed file-descripts.If it failed,return '-1'.

			//	Check status
			switch (result)
			{
				case 1:		//	Normal.
					//	Read data from this o_socket.
					result=recvfrom(o_socket,OUTPUT_BUF,BUF_LEN,0,(struct sockaddr*)&ADDR_PEER,(socklen_t*)&LEN_ADDR);
					//	Check data len from read by recvfrom().
					switch (result)
					{
						case 0:		//	Have nothing data.
							//	Not have to show error message.
							//fprintf(stderr,"Not have anymore data had be readed.\n");
							break;
						default:	//	Have some data.

							//	Debug check value from return by recvfrom().
							#ifdef DEBUG
								fprintf(stderr,"Value from return by recvfrom() - %d .\n",result);
							#endif

							//	View data.
							fprintf(stdout,"Messages : %s -- \n",OUTPUT_BUF);
							//	View address.
							fprintf(stdout,"ADDR : %s -- \n",
								inet_ntop(AF_INET,&ADDR_PEER.sin_addr,OUTPUT_BUF,BUF_LEN));

							/*
 						 	 *	Attention : 
 						 	 *		You can not as same time use the pointer OUTPUT_BUF and function inet_ntop() use 
 						 	 *	pointer OUTPUT_BUF as prameter.Because at the time when program entry the fprintf(),it will 
 						 	 *	execute function inet_ntop() before view data.So the data in OUTPUT_BUF will be overwritting 
 						 	 *	by inet_ntop().
 						 	 *		
 						 	 *
 						 	*/ 


							break;
					}
	
					break;
					
				case -1:	//	Have a error.Select() error.The matter is probable time out.
				
					//	Debug show error message for select().
					#ifdef DEBUG
						fprintf(stderr,"Select() error.\n");
					#endif

				default:	//	Don't care.

					//	Debug check value from return by select().
					#ifdef DEBUG
						fprintf(stderr,"select() returned,code %d\n",result);
					#endif

					;
	
			}			

		//	Because CONDITION of child process does not need changing by process,so signalaction not need IPC to synchronize.
		} while(CONDITION);

		//	Recycle resources and then quit function.
		//	Clear memory.
		free(OUTPUT_BUF);
		//	Clear fd_set.
		FD_CLR(o_socket,&r_set);
		//	Succeed to excuting.
		return 0;

	}
	//	Filed to get memory for buffer.
	else
		return -1;

}


static void SIG_INT_FUNC_F(int sig)
{
	//	Here must use IPC to synchronize for when CONDITION has been changing.
	//	Because commads 'EXIT#' and 'SWITCH#' will change value for CONDITION.
	//	Creat struct sembuf to execute IPC operation.
	struct sembuf sem_ops={
				.sem_num=0,
				.sem_op=-1,
				.sem_flg=SEM_UNDO,
			};

	//	int semop(int sem_id,struct sembuf* sem_ops,size_t num_sem_ops);
	/*
 	 * 	sem_id - IPC ID ;
 	 *	sem_ops - Operation set pointer,can use array to do a lot of actions ;
 	 *	num_sem_ops - Numbers of operation set (struct sembuf),it is not size of structure sembuf.
 	*/ 


	//	P
	(void)semop(IPC_ID,&sem_ops,1);
	//	Change CONDITION
	CONDITION=0;
	//	V
	sem_ops.sem_op=+1;
	(void)semop(IPC_ID,&sem_ops,1);
	//	Return.
	return;
}

static void SIG_INT_FUNC_C(int sig)
{
	//	Debug hint,got signal SIGINT.
	#ifdef DEBUG
		fprintf(stderr,"Child-process : Got signal SIGINT.\n");
	#endif
	//	CONDITION in child process and child process only check it.
	CONDITION=0;
	return;
}


static void SIG_ALRM(int sig)
{
	//	Parent process continue.
	return;
}

static void SIG_CHLD(int sig)
{
	//	When parent process has got a SIGCHLD from child process,it will be sending signal 'SIGINT' to itself.
	//	SIGINT will made parent process quit.
	(void)kill(getpid(),SIGINT);		//	Use getpid() function to get the PID is own itself.
	return;
}



static unsigned short int Carry_Port(char* IP_PORT)
{
	//	Check if is not null.
	if (IP_PORT)
	{
		//	Counter.
		unsigned short int Counter=0;
		unsigned short int Subscript1,Subscript2;
		Subscript1=Subscript2=0;
		//	Check ':',if it had been existed and then carry it on.
		while(*IP_PORT)
		{
			switch (*IP_PORT)
			{
				case ':':	//	Have ':'
					Counter++;
					Subscript2=Subscript1;
					//	Set position IP_PORT value to '\0'.
					//	Why changing value in the current position?
					//	Because element on this position is ':'.
					//	In the stuation if this array have other ':',the other ':' elements will be setting to '\0'.
					//	And then return default PORT,as that time IP-Address sure to be seted {IP'\0'}.
					//	So function inet_pton() can to translate IP Address.
					//
					//	But when input error,example that string "IP,PORT:",this function will be failed.
					*IP_PORT='\0';
					//	Update counter
					IP_PORT++;
					break;
				default:
						//	Traversal.
					Subscript1++;
					IP_PORT++;
			}
		}

		//	When this cycle finished,Subscript1 is equal the subscript-value of a element before '\0'.

		//	After traversal, check Subscript2.
		switch (Counter)
		{
			case 0:		//	Have not ':'
				return DFPORT;
			case 1:		//	Have ':'
				//	At this time,array position Subscript2 is the position of ':',Subscript1 is the position of '\0'.
				//	Pointer IP_PORT is direct to '\0'.
				//	Get the number length of port.
				Subscript1-=Subscript2;
				//	Direct the end number.
				IP_PORT-=1;
				//	Reset Subscript2 to save port.
				Subscript2=0;

				//	Carry port number.
				//	Check Subscript1 > 0 on there,because that in cycle while(*IP_PORT){},
				//	when cycle quit Subscript1 had not been updated.At that time,Subscript1=strlen(IP_PORT)-1.
				//	So,it is equal subscript-value of a element before '\0'.
				for (unsigned short int Right_value=1; Subscript1 > 0; --Subscript1,--IP_PORT)
				{
					//	Translate char value to int value.	value-'0'
					Subscript2+=(*IP_PORT-'0')*Right_value;
					Right_value*=10;	
				}

//				fprintf(stderr,"Check point IP_PORT.\n");
//				fprintf(stderr,"Subscript2= %d , *IP_PORT= %c\n",Subscript2,*IP_PORT);

			
				return Subscript2;	
			default:	//	Number of ':' is greater than 1,error.
				fprintf(stderr,"Error style with IP:PORT,use default port.\n");
				return DFPORT;
		}
		


	}
	else
		return DFPORT;
}



