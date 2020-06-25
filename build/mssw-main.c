#include"mssw.h"

int main(int argc,char* argv[])		//	Command line arguments.
{
	int state;

	if (argc < 2)
	{
		fprintf(stderr,"Argument error.Must provides the command line is equal 2.\n");
		return -1;
	}
	else
	{
		fprintf(stderr,"Process startting...\n");
	}

	if (strncmp(argv[1],"client",6) == 0)
		state=0;
	else if (strncmp(argv[1],"server",6) == 0)
		state=1;
	else
	{
		fprintf(stderr,"Arguments must are \"client\" and \"server\".\n");
		return -1;
	}

	if (CONTROL(state))
	{
		printf("Over...\n");
		return 0;
	}
	else
	{
		printf("Has error...\n");
		return -1;
	}


}
