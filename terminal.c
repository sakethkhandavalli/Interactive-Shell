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

char Cwd[1024];
int cpid,cflag;
int arrind;

int min(int a,int b)
{
	return a<b?a:b;
}
void siginthandler()
{
	return;
}
int greattest(char **argv,int ind)
{
	int i;
	for(i=0;i<ind;i++)
		if(!strcmp(argv[i],">"))
			return i;
	return 0;
}
int greatertest(char **argv,int ind)
{
	int i;
	for(i=0;i<ind;i++)
		if(!strcmp(argv[i],">>"))
			return i;
	return 0;
}
int lesstest(char **argv,int ind)
{
	int i;
	for(i=0;i<ind;i++)
		if(!strcmp(argv[i],"<"))
			return i;
	return 0;
}


int pipetest(char **argv,int ind,int* pipearr){
	int i;
	arrind=0;
	for(i=0;i<ind;i++){
		if(!strcmp(argv[i],"|")){
			pipearr[arrind]=i;
			arrind++;
		}
	}	
	if(arrind>0){
		return 1;
	}
	else
		return 0;
}

void init(char *tester)
{
	getcwd(Cwd, sizeof(Cwd));
	int i,ind=1;
	register struct passwd *pw;
	register uid_t uid;
	uid = geteuid();
	pw = getpwuid (uid);
	for(i=0;i<strlen(pw->pw_name);i++)
		tester[ind++] = (pw->pw_name)[i];
	strcat(tester,"@UBUNTU:~> ");
	return;
}

void changePrompt(char *tester,int index){
	int i;
	for(i=0;i<strlen(tester);i++)
		if(tester[i]==':')
			break;
	for(i++;i<strlen(tester);i++)
		tester[i]='\0';
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	for(i=0;i<strlen(cwd);i++)
		if(Cwd[i]!=cwd[i])
			break;
	if(i>strlen(Cwd)-1)
	{
		int j,indD=1;
		char a[1000]={"~/"};
		for(j=i;j<strlen(cwd);j++)
			a[indD++]=cwd[j];
		strcat(tester,a);
		strcat(tester,"> ");
	}
	else
	{
		strcat(tester,cwd);
		strcat(tester,"> ");
	}
	return;
}

void HomeCheck(char *optional,char *A)
{
	int i,ind=0,flag=0;
	for(i=0;i<strlen(optional);i++)
	{
		if(optional[i]=='~')
		{
			flag=1;
			break;
		}
		A[ind++]=optional[i];
	}
	if(flag)
	{
		strcat(A,Cwd);
		for(i++,ind+=8;i<strlen(optional);i++)
			A[ind++]=optional[i];
	}
	return;
}

void cd(char *s,char* tester){
	char A[1000]={'\0'};
	if(s==NULL){
		chdir(Cwd);
		changePrompt(tester,strlen(tester));
		return;
	}
	HomeCheck(s,A);
	if(chdir(A)<0){
		printf("Directory does not exist\n");
		return;
	}
	changePrompt(tester,strlen(tester));
	return;
}

void printFile(char* fname){
	struct stat fl;
	if(stat(fname,&fl) < 0){
		return;
	}
	if(S_ISDIR(fl.st_mode)){
		printf("%s%s%s%s",bold,blue,fname,reset);
	}
	else if((fl.st_mode & S_IXUSR)&&(fl.st_mode & S_IXGRP)&&(fl.st_mode & S_IXOTH)){
		printf("%s%s%s%s",bold,green,fname,reset);
	}
	else{
		printf("%s",fname);
	}
	return;
}

int getPerm(char* file){
	struct stat fileStat;
	if(stat(file,&fileStat) < 0){
		//printf("File is not present\n");
		return 1;
	}
	else{
		printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
		printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
		printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
		printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
		printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
		printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
		printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
		printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
		printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
		printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
	}
	return 0;
}

void getInfo(char *fname){
	struct group *grp;
	struct passwd *pwd;
	register uid_t uid;
	register gid_t gid;
	struct stat tm;
	struct stat fl;
	gid = getegid();
	uid = geteuid();
	int x=getPerm(fname);
	if(stat(fname,&fl) < 0){
		return;
	}

	if(x==0){
		printf("  %ld",fl.st_nlink);
		pwd = getpwuid(uid);
		printf("  %s",pwd->pw_name);
		grp = getgrgid(gid);
		printf("  %s",grp->gr_name);
		printf("  %ld",fl.st_size);
		stat(fname, &tm);
		char* s=ctime(&tm.st_mtime);
		s[strlen(s)-1]='\0';
		printf("  %s",s);
		printf("  ");
		printFile(fname);
		printf("\n");
	}
	return;
}

void getLs(DIR *dp , int type){
	struct dirent *file;
	while(1){
		file = readdir(dp);
		if(file==NULL)
			break;
		if(type==0){
			char* fname=file->d_name;
			if(fname[0]!='.'){
				printFile(fname);
				printf("  ");
			}
		}
		else if(type==1){
			printFile(file->d_name);
			printf("  ");
		}
		else if(type==2){
			char* fname=file->d_name;
			if(fname[0]!='.'){
				getInfo(fname);
			}
		}
		else{
			char* fname=file->d_name;
			getInfo(fname);
		}
	}
	if(type==0 || type==1)
		printf("\n");
	closedir(dp);
	return;
}


void ls(char** argv , int argind){
	char cwd[1024];
	char* name=NULL;
	int type=0,ind=1;
	getcwd(cwd, sizeof(cwd));
	while(ind < argind && argv[ind]!=NULL){
		if(strlen(argv[ind])==2 && argv[ind][0]=='-' && argv[ind][1]=='a'){
			if(type==2)
				type=3;
			else
				type=1;
		}
		else if(strlen(argv[ind])==2 && argv[ind][0]=='-' && argv[ind][1]=='l'){
			if(type==1)
				type=3;
			else
				type=2;
		}
		else if(strlen(argv[ind])==3 && argv[ind][0]=='-' && ((argv[ind][1]=='l' && argv[ind][2]=='a') || (argv[ind][1]=='a' && argv[ind][2] == 'l')))
			type=3;
		else{
			name=argv[ind];
		}
		ind++;
	}
	int t;
	struct stat fileStat;
	if(name!=NULL){
		if(stat(name,&fileStat) < 0)
			t=0;
		if (S_ISDIR(fileStat.st_mode)){
			DIR *dp=NULL;
			if(name!=NULL){
				dp = opendir(name);
				chdir(name);
			}
			else
				dp = opendir(cwd);
			if(dp==NULL){
				printf("Error : Could not open the directory");
				return;
			}
			getLs(dp,type);
		}
		else{
			getInfo(name);
		}
	}
	else{
		DIR *dp=NULL;
		dp=opendir(cwd);
		getLs(dp,type);
	};
	chdir(cwd);
	return;
}		

void echo(char** argv){
	int t=0,ind=1;
	if(argv[ind]==NULL){
		printf("\n");
		return;
	}
	if(argv[ind][0]=='"')
		t=1;
	else if(argv[ind][0]==39)
		t=2;
	while(argv[ind] != NULL){
		if(argv[ind][0]=='"' || argv[ind][strlen(argv[ind])-1]=='"')
		{
			if(t==2)
				printf("%s\n",argv[ind]);
			else{
			if(t==1)
				if(argv[ind][strlen(argv[ind])-1]=='"')
				{
					argv[ind][strlen(argv[ind])-1]='\0';
				}
				if(argv[ind][0]=='"')
					printf("%s ",argv[ind]+1);
				else
					printf("%s ",argv[ind]);
			}
		}
		else if(argv[ind][0]==39 || argv[ind][strlen(argv[ind])-1]==39)
		{
			if(t==1)
				printf("%s\n",argv[ind]);
			else{
			if(t==2)
				if(argv[ind][strlen(argv[ind])-1]==39)
					argv[ind][strlen(argv[ind])-1]='\0';
				if(argv[ind][0]==39)
					printf("%s ",argv[ind]+1);
				else
					printf("%s ",argv[ind]);
			}
		}
		else
			printf("%s ",argv[ind]);
		ind++;
	}
	printf("\n");
	return;
}

void pinfo(char** argv,int Pind){
	int p,ind=1;
	if(Pind == 1)
		p = getpid();
	else
		p = atoi(argv[ind]);
	char pep[10],parser[1000];
	char dest[] = "/proc/";
	sprintf(pep, "%d", p);
	strcat(dest,pep);
	strcat(dest,"/stat");
	FILE * fp = fopen(dest,"r");
	if(fp==NULL){
		printf("Process does not exist\n");
	}
	else{
		fscanf(fp," %[^\n]s",parser);
		char *hey = strtok(parser," \t");
		printf("Pid: %s\n",hey);
		hey = strtok(NULL," \t");
		char info[1024]="/proc/",ans[1024],pepper[10];
		sprintf(pepper, "%d", p);
		strcat(info,pepper);
		strcat(info,"/exe");
		int len = readlink(info,ans,sizeof(char)*500);
		ans[len]='\0';
		printf("Executable: %s\n",ans);
		hey = strtok(NULL," \t");
		printf("Status: %s\n",hey);
		int x=22;
		while(x)
		{
			hey = strtok(NULL," \t");
			x--;
		}
		printf("Virtual Memory: %s\n",hey);
	}
	return;
}

int main(){
	int pid, i,flag=0;
	size_t lenn;
	int cfl=0;
	char *a = (char *)malloc(sizeof(char)*1024);
	struct dirent *file;
	char tester[1000]={'<'};
	init(tester);
	int pip,fd,pip1,Fd;
	int ct = 0,status;
	pid_t child[100], chind=0;
	int Flag[100] = {};
	signal(SIGINT,siginthandler);
	//signal(SIGCHLD,SIG_IGN);
	signal(SIGSTOP,siginthandler);
	int stdou = dup(1);
	int stdi = dup(0);
	while(1){
		int y=waitpid(-1, &status, WNOHANG);
		if(y>0){
			if(WIFEXITED(status))
				printf("\nProcess with pid : %d has exited normally\n",y);
			else if(WIFSIGNALED(status))
				printf("\nProcess with pid : %d was terminated by a signal\n",y);
		}
		flag=0;
		char* cmd;
		char **argv;
		if(cfl!=1)
			printf("%s%s%s%s",bold,red,tester,reset);
		cfl=0;
		scanf(" %[^\n]s",a);
		for(i=0;i<strlen(a);i++)
			if(a[i]=='&') flag=1;
		char *str = strtok(a," \t");
		cmd = str;
		argv = (char **)malloc(sizeof(char *)*1024);
		argv[0] = str;
		argv[1]=NULL;
		int ind = 1;
		int pipearr[100],x;
		int pstatus,f;
		while(str!=NULL){
			str=strtok(NULL," \t");
			if(str!=NULL){
				if(strcmp(str,"&")){
					argv[ind] = str;
					ind++;
				}
			}
		}
		pip1 = lesstest(argv,ind) ;
		pip = greattest(argv,ind);
		if(pip && pip1)
		{
			int F = fork();
			if(!F)
			{
				if(access(argv[pip+1], F_OK) != -1) 
					fd = open(argv[pip+1],O_WRONLY | O_TRUNC, 0644);
				else
					fd = open(argv[pip+1],O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(fd == -1)
					perror("failed to open the file");
				close(1);
				printf("%d\n",fd);
				if(dup2(fd, 1) == -1)
					perror("failed dup3");
				Fd = open(argv[pip1+1],O_RDONLY, 0644);
				if(Fd == -1)
					perror("file");
				close(0);
				if(dup2(Fd, 0) == -1)
					perror("dup3");
				char ** argvt = (char **)malloc(sizeof(char *)*10);
				int s;
				close(fd);
				for(s=0;s<min(pip,pip1);s++)
					argvt[s] = argv[s];
				execvp(argvt[0],argvt);
			}
			else
				wait(NULL);
			continue;
		}
		else if(pip = greattest(argv,ind))
		{
			int f = fork();
			if(!f)
			{
				if(access(argv[pip+1], F_OK) != -1) 
					fd = open(argv[pip+1],O_WRONLY | O_TRUNC, 0644);
				else
					fd = open(argv[pip+1],O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(fd == -1)
					perror("failed to open the file");
				close(1);
				if(dup2(fd, 1) == -1)
					perror("failed dup3");
				char ** argvt = (char **)malloc(sizeof(char *)*pip);
				char *tcmd;
				tcmd = argv[0];
				int s;
				close(fd);
				for(s=0;s<pip;s++)
					argvt[s] = argv[s];
				execvp(argvt[0],argvt);
			}
			else
			{
				wait(NULL);
				close(stdou);
			}
			continue;
		}
		else if(pip = greatertest(argv,ind))
		{
			int f = fork();
			if(!f)
			{
				if(access(argv[pip+1], F_OK) != -1) 
					fd = open(argv[pip+1],O_APPEND | O_WRONLY, 0644);
				else
					fd = open(argv[pip+1],O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(fd == -1)
					perror("failed to open the file");
				close(1);
				if(dup2(fd, 1) == -1)
					perror("failed dup3");
				char ** argvt = (char **)malloc(sizeof(char *)*pip);
				char *tcmd;
				tcmd = argv[0];
				int s;
				close(fd);
				for(s=0;s<pip;s++)
					argvt[s] = argv[s];
				execvp(argvt[0],argvt);
			}
			else
			{
				wait(NULL);
				close(stdou);
			}
			continue;
		}
		else if(pip = lesstest(argv,ind))
		{
			pid_t  f = fork();
			if(f==0)
			{
				fd = open(argv[pip+1],O_RDONLY, 0644);
				if(fd == -1)
					perror("file");
				close(0);
				if(dup2(fd, 0) == -1)
					perror("dup3");
				char *argvt[100]={NULL};
				int s;
				close(fd);
				for(s=0;s<pip;s++)
				{
					argvt[s] = argv[s];
					argvt[s][strlen(argvt[s])] = '\0';
				}
				for(s=0;s<=pip;s++)
					printf("hello: %s",argvt[s]);
				argvt[0][strlen(argvt[0])] = '\0';
				execvp(argvt[0],argvt)==-1;
				{
					printf("Error\n");
					kill(getpid(),SIGKILL);
				}
			}
			else if(f!=-1)
			{
				wait(NULL);
				close(stdi);
			}
			continue;
		}
		else if(pipetest(argv,ind,pipearr))
		{
			int fildes[2];
			if(pipe(fildes) != 0) 
				perror("pipe failed");
			int num=0;
			int ptr=pipearr[num];
			int nptr=0;
			
			int flag=0;
			while(num!=arrind+1){
				f=fork();
				if(f<0)
					perror("fork failed");
				else if(f==0){
					if(num!=0 && num!=arrind){
						nptr=pipearr[num-1]+1;
						ptr=pipearr[num];
					}
					else if(num==arrind)
					{
						nptr = pipearr[num-1]+1;
						ptr = ind;
					}
					char ** argvt = (char **)malloc(sizeof(char *)*ptr);
					int s;
					for(s=0;s<ptr-nptr;s++)
						argvt[s] = argv[s+nptr];
					if(num!=arrind)
					{
						//close(fildes[0]);
						//close(1);
						if(dup2(fildes[1], 1) == -1)
							perror("dup3 fail1233444");
						//close(fildes[1]);
					}
					else if(num==arrind)
					{
						close(1);
						if(dup2(stdou, 1) == -1)
							perror("failed dup4 stdou");
					}
					for(x=nptr;x<ptr;x++)
						argvt[x-nptr][strlen(argvt[x-nptr])] = '\0';
					argvt[ptr-nptr] = NULL;
					if(execvp(argvt[0],argvt)<0)
						printf("Error bey !!");
				}
				else
				{
					waitpid(f,&pstatus,WUNTRACED);
					close(fildes[1]);
					//close(0);
					if(dup2(fildes[0], 0) == -1)
						perror("dup1 fail stdi");
					//close(fildes[0]);
				}
				num++;
				}	
			close(1);
			if(dup2(stdou, 1) == -1)
				perror("failed dup7 stdou");
			close(0);
			if(dup2(stdi, 0) == -1)
				perror("failed dup9");
			continue;
		}
		if(strcmp(cmd,"cd")==0){
			cfl=1;
			if(ind<=2){
				if(ind!=1)
					cd(argv[1],tester);
				else
					cd(NULL,tester);
			}
			else
				printf("cd takes only one argument\n");
			printf("%s%s%s%s",bold,red,tester,reset);
			continue;
		}
		else if(strcmp(cmd,"echo")==0){
			echo(argv);
			continue;
		}
		else if(strcmp(cmd,"pinfo")==0){
			pinfo(argv,ind);
			continue;
		}
		else if(strcmp(cmd,"ls")==0){
			ls(argv,ind);
			continue;
		}
		else if(!strcmp(cmd,"pwd"))
		{
			char cwd[1024];
			getcwd(cwd, sizeof(cwd));
			printf("%s\n",cwd);
			continue;
		}
		else if(!strcmp(cmd,"mkdir"))
		{
			mkdir(argv[1],0777);
			continue;
		}
		else if(!strcmp(cmd,"nightswatch"))
		{
			nights(argv,ind);
			continue;
		}
		else if(!strcmp(cmd, "setenv"))
		{
			if(ind<2 || ind>3)
				perror("Mistake in number of args..!");
			else if(ind==2)
				if(setenv(argv[1],"",1))
					perror("Error in setting");
			else
				if(setenv(argv[1],argv[2],1))
					perror("Error in setting");
			continue;
		}
		else if(!strcmp(cmd, "unsetenv"))
		{
			if(ind != 2)
				perror("Mistake in number of args");
			else
				if(unsetenv(argv[1])==-1)
					printf("Error in unsetting");
			continue;
		}
		else if(!strcmp(cmd, "jobs"))
		{
			int s;
			for(s=0;s<chind;s++)
			{
				if(Flag[s]) continue;
				char DONE[10] = {"Done\0"};
				char RUNNING[20] = {"Running\0"};
				char SLEEPING[20] = {"Sleeping\0"};
				char STOPPED[20] = {"Stopped\0"};
				char pep[10],parser[1000];
				char dest[] = "/proc/";
				FILE * fp;
				sprintf(pep, "%d", child[s]);
				strcat(dest,pep);
				strcat(dest,"/stat");
				fp = fopen(dest,"r");
				fscanf(fp," %[^\n]s",parser);
				char *hey = strtok(parser," \t");
				hey = strtok(NULL," \t");
				char * mine = strtok(NULL," \t");
				if(!strcmp(mine,"Z") || !strcmp(mine,"D"))
					Flag[s] = 1;
				hey[strlen(hey)-1]='\0';
				if(!strcmp(mine,"Z"))
					mine = STOPPED;
				if (!strcmp(mine,"D"))
					mine = DONE;
				else if(!strcmp(mine,"R"))
					mine = RUNNING;
				else
					mine = SLEEPING;
				printf("[%d]  %s  %s  [%d]\n",s+1,mine,hey+1,child[s]);
			}
			continue;
		}
		else if(!strcmp(cmd, "kjob"))
		{
			if(ind != 3)
				perror("Wrong number of args");
			else
			{
				int tempct = 0,s=0;
				while(tempct<atoi(argv[1]))
				{
					if(Flag[s]) tempct++;
					s++;
				}
				kill(child[tempct-1],atoi(argv[2]));
			}
			continue;
		}
		else if(!strcmp(cmd, "overkill"))
		{
			if(ind!=1)
				perror("too many errors");
			else
			{
				int s;
				for(s=0;s<chind;s++)
					kill(child[s],9);
			}
			continue;
		}
		else if(!strcmp(cmd, "fg"))
		{
			if(ind != 2)
				perror("Please check the number of args");
			else
			{
				if(atoi(argv[1])>chind)
					perror("Id doesnt exist");
				else
				{
					int tempct = 0,s=0;
					while(tempct<atoi(argv[1]))
					{
						if(Flag[s]) tempct++;
						s++;
					}
					int statuss;
					waitpid(child[tempct-1],&statuss,WUNTRACED);
					Flag[tempct-1] = 1;
				}
			}
			continue;
		}
		else if(!strcmp(cmd, "bg"))
		{
			if(ind != 2)
				perror("Please check the number of args");
			else
			{
				if(atoi(argv[1])>chind)
					perror("Id doesnt exist");
				char Pep[10],Parser[1000];
				char Dest[] = "/proc/";
				FILE * Fp;
				sprintf(Pep, "%d", child[atoi(argv[1]-1)]);
				strcat(Dest,Pep);
				strcat(Dest,"/stat");
				Fp = fopen(Dest,"r");
				fscanf(Fp," %[^\n]s",Parser);
				int S;
				char * name = strtok(Parser," \t");
				for(S=0;S<1;S++)
					name = strtok(NULL," \t");
				name[strlen(name)-1] = '\0';
				pid_t bg = fork();
				if(!bg)
				{
					char *argV[3];
					argV[0] = name+1;
					argV[1] = NULL;
					execvp(argV[0],argV);
				}
				else
				{
					kill(child[atoi(argv[1]-1)], 9);
				}
			}

		}
		else if(!strcmp(cmd,"quit"))
		{

			int s;
			for(s=0;s<chind;s++)
				kill(child[s],9);
			return 0;
		}
		pid = fork();
		if(pid == 0){
			// Child process
			cmd[strlen(cmd)] = '\0';
			int x;
			for(x=0;x<ind;x++)
				argv[x][strlen(argv[x])] = '\0';
			argv[ind] = NULL;
			if(execvp(cmd,argv)<0)
			{
				printf("Command does not exist\n");
				exit(0);
			}
				
		}
		else{
			//Parent process
			if(flag==0)
			{
				int x = waitpid(pid,&status,WUNTRACED);
			}
			child[chind++] = pid;	
		}
	}
	return 0;
}
