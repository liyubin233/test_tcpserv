#include	"unp.h"
#include	"myerr.h"

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while((n = read(sockfd, buf, MAXLINE)) > 0) {
		Writen(sockfd, buf, n);
	}
	if(n < 0 && errno == EINTR) {
		goto again;
	}
	else if(n < 0) {
		err_sys("str_echo: read error");
	}
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
#if 0
	while(pid = waitpid(-1,&stat,WNOHANG) >0) {
		printf("child %d ternimated\n", pid);
	}
#else
	pid = wait(&stat);
	printf("child %d terminated\n",pid);
#endif

	return;
}
int main(int argc ,char **argv)
{
	int connfd, listenfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char buff[MAXLINE];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	Signal(SIGCHLD, sig_chld); /* used to deal with child :close it*/

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	
	while(1) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		printf("connection from %s ,port %d\n", 
			Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
			ntohs(cliaddr.sin_port));
		if((childpid = fork()) == 0) { /* come to child */
			Close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
