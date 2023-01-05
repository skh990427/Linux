#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

void Calculator(char* in, char* out);
int readLine(int fd, char* str);

/*계산해주는 서버 프로그램*/
int main()
{
    int listenfd, connfd, clientlen;
    char inmsg[MAXLINE], outmsg[MAXLINE];
    struct sockaddr_un serverUNIXaddr, clientUNIXaddr;

    signal(SIGCHLD, SIG_IGN);
    clientlen = sizeof(clientUNIXaddr);

    listenfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXaddr.sun_family = AF_UNIX;

    strcpy(serverUNIXaddr.sun_path, "convert");
    unlink("convert"); //해당 파일이 존재하면 삭제
    bind(listenfd, (struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));

    listen(listenfd, 5); //최대 5개까지 대기를 기다림

    while(1)
    {
	connfd = accept(listenfd, (struct sockaddr *)&clientUNIXaddr, &clientlen);
	if(fork() == 0)
	{
	    readLine(connfd, inmsg);
	    Calculator(inmsg, outmsg);
	    write(connfd, outmsg, strlen(outmsg)+1);
	    close(connfd);
	    exit(0);
	}
	else
	{
	    close(connfd);
	}
    }
}

void Calculator(char* in, char* out)
{
    int n[2], iresult;
    double d_result;

    char* ptr1 = strchr(in, '+');
    char* ptr2 = strchr(in, '-');
    char* ptr3 = strchr(in, '*');
    char* ptr4 = strchr(in, '/');

    if(ptr1 != NULL)
    {
	n[0] = atoi(strtok(in, " +"));
	n[1] = atoi(strtok(NULL, " +"));

	iresult = n[0] + n[1];
	sprintf(out, "%d", iresult);
    }
    else if(ptr2 != NULL)
    {
	n[0] = atoi(strtok(in, " -"));
	n[1] = atoi(strtok(NULL, " -"));

	iresult = n[0] - n[1];
	sprintf(out, "%d", iresult);
    }
    else if(ptr3 != NULL)
    {
	n[0] = atoi(strtok(in, " *"));
	n[1] = atoi(strtok(NULL, " *"));

	iresult = n[0] * n[1];
	sprintf(out, "%d", iresult);
    }
    else if(ptr4 != NULL)
    {
	n[0] = atoi(strtok(in, " /"));
	n[1] = atoi(strtok(NULL, " /"));

	d_result = (double)n[0] / (double)n[1];
	sprintf(out, "%.2lf", d_result);
    }
}

int readLine(int fd, char* str)
{
    int n;
    do {
	n = read(fd, str, 1);
    } while(n > 0 && *str++ != '\0');
    return(n > 0);
}
