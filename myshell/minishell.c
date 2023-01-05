#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char cmd[100];
char ecmd[100];

void cmd_ls(int count, char* argv[]);
void cmd_pwd(int count);
void cmd_cd (int count, char* argv[]);
void cmd_cat(int count, char* argv[]);
void cmd_cp(int count, char* argv[]);
void cmd_mv(int count, char* argv[]);
void cmd_echo(char* argv[]);

int main(int argc, char* argv[])
{
    while(1)
    {
	printf(">> ");
	fgets(cmd, 100, stdin);
	cmd[strlen(cmd)-1] = '\0';

	strcpy(ecmd, cmd);
	

	char* ptrx = strtok(cmd, " ");

	int count = 0;
	argv[0] = ptrx;
	ptrx = strtok(NULL, " ");

	while(ptrx != NULL)
	{
	    count++;
	    argv[count] = ptrx;

	    ptrx = strtok(NULL, " ");
	}

	if(strcmp(argv[0], "ls") == 0)
	{
	    cmd_ls(count, argv);
	}
	else if(strcmp(argv[0], "pwd") == 0)
	{
	    cmd_pwd(count);
	}
	else if(strcmp(argv[0], "exit") == 0)
	{
	    if(count == 0)
		exit(0);
	    else
	    {
		printf("사용법 : exit만 입력\n");
		continue;
	    }
	}
	else if(strcmp(argv[0], "cd") == 0)
	{
	    cmd_cd(count, argv);
	}
	else if(strcmp(argv[0], "cat") == 0)
	{
	    cmd_cat(count, argv);
	}
	else if(strcmp(argv[0], "mkdir") == 0)
	{
	    int result;

	    if(count == 1)
	    {
		result = mkdir(argv[1], 0777);
	    }
	    else
	    {
		printf("사용법 : mkdir [file]\n");
	    }

	    if(result != 0)
	    {
		printf("폴더 생성 실패\n");
	    }
	}
	else if(strcmp(argv[0], "cp") == 0)
	{
	    cmd_cp(count, argv);
	}
	else if(strcmp(argv[0], "mv") == 0)
	{
	    cmd_mv(count, argv);
	}
	else if(strcmp(argv[0], "echo") == 0)
	{
	    char ecmd2[100];
	    char ecmd3[100];
	    strcpy(ecmd2, ecmd);
	    strcpy(ecmd3, ecmd);
	    int e_count = 0;

	    for(int i = 1 ; ecmd[i] != 0 ; i++)
	    {
		if(ecmd[i] == '\"') e_count++;
	    }

	    if(e_count != 2)
	    {
		printf("사용법 : echo \"문자열\" >> [텍스트 파일], echo \"문자열\" > [텍스트 파일]\n");
		continue;
	    }

	    strtok(ecmd, "\"");
	    argv[1] = strtok(NULL, "\"");

	    char* eptr_2 = strtok(ecmd2, " ");
	    while(eptr_2 != NULL)
	    {
		argv[2] = eptr_2;
		eptr_2 = strtok(NULL, " ");
	    }
	    if(strstr(ecmd3, "\" >> "))
	    {
		FILE *fp;

		fp = fopen(argv[2], "a");
		fputs(argv[1], fp);
		fputs("\n", fp);

		fclose(fp);
	    }
	    else if(strstr(ecmd3, "\" > "))
	    {
		FILE *fp;

		fp = fopen(argv[2], "w");
		fputs(argv[1], fp);
		fputs("\n", fp);

		fclose(fp);
	    }
	    else
	    {
		printf("사용법 : echo \"문자열\" >> [텍스트 파일], echo \"문자열\" > [텍스트 파일]\n");
	    }

	}
	else
	    printf("알수 없는 명령어입니다.\n");
    }

    return 0;
}

void cmd_ls(int count, char* argv[])
{
    DIR *dp;
    char *dir;
    struct dirent *d;

    if(count == 0)
	dir = ".";
    else if(count == 1)
	dir = argv[1];
    else
    {
	printf("사용법 : ls OR ls [검색하고싶은 파일 ex)ls ..]\n");
	return;
    }

    if((dp = opendir(dir)) == NULL)
    {
	printf("디렉토리를 찾을 수 없습니다.\n");
	printf("사용법 : ls OR ls [검색하고싶은 파일 ex)ls ..]\n");
	return;
    }
 
    while((d = readdir(dp)) != NULL)
    {
	if(d->d_name[0] != '.')
	    printf("%s\t", d->d_name);
    }
    printf("\n");
   
    closedir(dp);
}

void cmd_pwd(int count)
{
    if(count == 0)
    {
    char path[PATH_MAX];
    printf("%s\n", getcwd(path, PATH_MAX));
    }
    else
    {
	printf("사용법 : pwd만 입력\n");
    }
}

void cmd_cd(int count, char* argv[])
{
    if(count == 0)
	chdir(getenv("HOME"));

    else if(count == 1)
    {
	if(chdir(argv[1]) == -1)
	    printf("해당 디렉토리가 없습니다\n");
    }
    else
	printf("사용법 : cd [dir]\n");
}

void cmd_cat(int count, char* argv[])
{
    char str;
    int fd;

    if(count == 1)
    {
	if((fd = open(argv[1], O_RDONLY)) == -1)
	{
	    printf("파일을 찾을 수 없거나 열 수 없습니다\n");
	    return;
	}

	while(read(fd, &str, 1))	//str을 1바이트씩 읽어서 모니터에 출력
	{
	    write(1, &str, 1);
	}

	close(fd);
    }
    else
    {
	printf("사용법 : cat [file]\n");
    }
}

void cmd_cp(int count, char* argv[])
{
    if(count == 2)
    {
	FILE *fp1, *fp2;
	char c;

	fp1 = fopen(argv[1], "r");
	if(fp1 == NULL)
	{
	    fprintf(stderr, "파일 %s 열기 오류\n", argv[1]);
	    return;
	}
	
	fp2 = fopen(argv[2], "w");
	while((c = fgetc(fp1)) != EOF)
	    fputc(c, fp2);

	fclose(fp1);
	fclose(fp2);
    }
    else
    {
	printf("사용법 : cp [원본파일] [사본파일]\n");
	return;
    }
}

void cmd_mv(int count, char* argv[])
{
    if(count == 2)
    {
	cmd_cp(count, argv);
	unlink(argv[1]);
    }
    else
    {
	printf("사용법 : mv [원본파일] [변경하고싶은 파일명]\n");
	return;
    }
}
