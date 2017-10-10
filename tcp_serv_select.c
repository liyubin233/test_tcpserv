#include	"unp.h"
#include	"myerr.h"

int main(int argc, char **argv)
{
	int i, listenfd, sockfd, connfd, maxfd, maxi;
	struct sockaddr_in cliaddr, servaddr;
	char buf[MAXLINE],buff[MAXLINE];
	fd_set rset,allset;
	ssize_t n;
	int nready,client[FD_SETSIZE];
	socklen_t clilen;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
		
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	
	maxfd = listenfd;
	maxi = -1;

	for(i = 0; i < FD_SETSIZE; i ++) {
		client[i] = -1; /* -1 indicate available entry */
	}
	FD_ZERO(&allset); /* init the fd_set */
        FD_SET(listenfd, &allset); /*first we add the listenfd to fd_set */

	
	for( ; ; ) {
		rset = allset;
		nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);
	
		if(FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
                        printf("new client: %s, port %d\n",
                                        Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                                        ntohs(cliaddr.sin_port));

			for(i = 0 ;i < FD_SETSIZE; i ++) {
				if(client[i] < 0) {
					/* add connfd to client */
					client[i] = connfd; 
					break;
				}
			}
			if(i == FD_SETSIZE) {
				err_quit("too many clients !");
			}
			
			FD_SET(connfd, &allset);
			if(connfd > maxfd) {
				maxfd = connfd;
			}
			if(i > maxi) {
				maxi = i; /* max index in client[] array*/
			}

			if(--nready <= 0) { /* no more readable descriptors*/
				printf("no more descrip\n");
				continue;
			}
		}

		for(i = 0; i <= maxi; i++) {
			if((sockfd = client[i]) < 0) {
				printf("BAD !\n");
				continue;
			}
			if(FD_ISSET(sockfd, &rset)) {
				/* connection closed by client*/
				if((n = Read(sockfd, buf, MAXLINE)) == 0) {
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					printf("connect closed by client!\n");
				} else {
					Writen(sockfd, buf, n);
				}
				
				if(--nready <= 0) {
					break;
				}
			}
		}
	}
}
