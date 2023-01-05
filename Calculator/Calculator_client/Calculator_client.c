#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

int readLine(int fd, char* str);

/*계산해주는 클라이언트 프로그램*/
int main()
{
    char yorn;
    while(1)
    {
	int clientfd, result;
	char cpy_msg[MAXLINE];
	char inmsg[MAXLINE], outmsg[MAXLINE];
	struct sockaddr_un serverUNIXaddr;

	clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;
	strcpy(serverUNIXaddr.sun_path, "convert");

	do {
	    result = connect(clientfd, (struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));
	    if(result == -1)
		sleep(1);
	}while(result == -1);

	printf("계산할 식 입력 : ");
	fgets(inmsg, MAXLINE, stdin);
	inmsg[strlen(inmsg)-1] = '\0';

	strcpy(cpy_msg, inmsg);

	char* ptr = strtok(cpy_msg, " ");
	ptr = strtok(NULL, " ");
	int count = 2;

	if(*ptr != '+' && *ptr != '-' && *ptr != '*' && *ptr != '/')
	{
	    printf("사용법 : 두개의 정수와 한개의 연산자를 입력해주세요 ex) 1 + 1\n");
	    continue;
	}
	while(ptr != NULL)
	{
	    count++;
	    ptr = strtok(NULL, " ");
	}
	if(count > 4)
	{
	    printf("사용법 : 두개의 정수와 한개의 연산자를 입력해주세요 ex) 1 + 1\n");
	    continue;
	}

	write(clientfd, inmsg, strlen(inmsg)+1);

	readLine(clientfd, outmsg);
	printf("결과값 : %s\n", outmsg);

	printf("계속 하시겠습니까?(y/n) : ");
	scanf("%c", &yorn);
	getchar();

	if(yorn == 'n')
	{
	    break;
	    close(clientfd);
	}
	else if(yorn = 'y')
	    continue;
    }
}

int readLine(int fd, char* str)
{
    int n;
    do {
	n = read(fd, str, 1);
    }while(n > 0 && *str++ != '\0');
    return (n > 0);
}
