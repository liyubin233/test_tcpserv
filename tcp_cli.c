#include	"unp.h"
#include	"myerr.h"

#if 0
/* this function can not deal with some problem,
 * so we use a new str_cli 
 */
static void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while(Fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, 1);
		sleep(1);
		Writen(sockfd, sendline, strlen(sendline));
		if( Readline(sockfd, recvline, MAXLINE) == 0) {
			err_quit("str_cli: server terminaled prematurely");
		}
		Fputs(recvline, stdout);
	}
}
#endif

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];
	
	FD_ZERO(&rset);
	while(1) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset); /* add the socket and write to fd_set */
		maxfdp1 = max(fileno(fp), sockfd)+1; /* attention this +1 */
		Select(maxfdp1, &rset, NULL, NULL, NULL);/* check read */
		
		if(FD_ISSET(sockfd, &rset)) { /* socket is readable */
	                if( Readline(sockfd, recvline, MAXLINE) == 0) {
        	                err_quit("str_cli: server terminaled prematurely");
               		 }
                	Fputs(recvline, stdout);
		}

		if(FD_ISSET(fileno(fp), &rset)) { /* you write sth in cli */
			if(Fgets(sendline, MAXLINE, fp) == NULL){
				return;
			}
			Writen(sockfd, sendline, strlen(sendline));
		}
	}
}

int main(int argc, char ** argv)
{
	int i, sockfd;
	struct sockaddr_in servaddr;
	
	i = 0;
	if(argc != 2) {
		err_quit("usage: tcp_cli <IPaddress>!");
	}
#ifdef MUTICHILD
	for(i = 0; i<5; i++) {
#endif
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	Connect(sockfd, (SA *) &servaddr ,sizeof(servaddr));
#ifdef MUTICHILD	
	}
#endif

	str_cli(stdin, sockfd);
	
	exit(0);
	
}
