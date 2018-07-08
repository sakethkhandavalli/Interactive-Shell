#include<time.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/utsname.h>
#include<string.h>
#include <pwd.h>
#include<grp.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<fcntl.h>
#include<stdlib.h>
#include <math.h>
#include<sys/wait.h>
#include<signal.h>
#include"bonus.h"


#define red     "\x1b[31m"
#define green   "\x1b[32m"
#define yellow  "\x1b[33m"
#define blue    "\x1b[34m"
#define magenta "\x1b[35m"
#define cyan    "\x1b[36m"
#define reset   "\x1b[0m"
#define bold    "\033[1m"
#define no_bold "\033[22m"

void nights(char **argv,int ind)
{
	int pid = fork();
	if(!pid)
	{
		if(ind!=4)
		{
			printf("See usage\n");
			return;
		}
		if(!strcmp(argv[3],"dirty"))
		{
			int time = atoi(argv[2]);
			while(1)
			{
				FILE * fp = fopen("/proc/meminfo","r");
				if(fp<0) printf("There is an error\n");
				int x=0;
				char test[1024];
				for(x=0;x<17;x++)
					fscanf(fp," %[^\n]s",test);
				char *tok = strtok(test," \t");
				tok = strtok(NULL," \t");
				printf("%s kB\n",tok);
				sleep(time);
			}
		}
		else
		{
			int time = atoi(argv[2]),i;
			FILE * fp = fopen("/proc/interrupts","r");
			if(fp<0) printf("There is an error\n");
			char test[1024];
			fscanf(fp," %[^\n]s",test);
			int ct=1;
			char *tok = strtok(test," \t");
			while(tok !=NULL)
			{
				tok = strtok(NULL," \t");
				ct++;
			}
			ct--;
			for(i=0;i<ct;i++)
				printf("CPU%d\t",i);
			printf("\n");
			while(1)
			{
				fp = fopen("/proc/interrupts","r");
				if(fp<0) printf("There is an error\n");
				int x=0;
				char test[1024];
				fscanf(fp," %[^\n]s",test);
				fscanf(fp," %[^\n]s",test);
				fscanf(fp," %[^\n]s",test);
				char *tok = strtok(test," \t");
				for(x=0;x<ct;x++)
				{
					tok = strtok(NULL," \t");
					printf("%s\t",tok);
				}
				printf("\n");
				sleep(time);
			}
		}
	}
	else
	{
		while(getchar()!='q')
			continue;
		kill(pid, SIGKILL);
	}
}
