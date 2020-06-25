#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>

int main(void)
{

	pid_t PID;

	PID=fork();

	if (PID == 0)
	{

		PID=waitpid(-1,(int*)0,NULL);	//	Get child pid.
		printf("Child pid = %d\n",PID);
		PID=wait();
		printf("Child process has been finished...PID = %d\n",PID);
	}
	else
	{
		PID=getpid();
		fprintf(stderr,"In child process,PID=%d\n",PID);
		PID=getppid();
		fprintf(stdout,"Father process PID=%d \nChild process Exitting...\n",PID);	
		exit(1);
	}
	return 0;
}
