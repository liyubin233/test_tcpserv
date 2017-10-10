CC = gcc
CFLAGS = -Ilib -g -O2 -D_REENTRANT -Wall
LIBS = ./libunp.a -lpthread

PROGS = tcp_serv tcp_cli tcp_serv_select 

all:    ${PROGS}

tcp_serv:  tcp_serv.o
	$(CC) ${CFLAGS} -o $@ tcp_serv.o ${LIBS}

tcp_cli:  tcp_cli.o libunp.a
	${CC} ${CFLAGS} -o $@ tcp_cli.o ${LIBS}

tcp_serv_select:tcp_serv_select.o 
	${CC} ${CFLAGS} -o $@ tcp_serv_select.o ${LIBS}

clean:
	rm -rf  *.o *.lc
clean_app:
	rm -rf $(PROGS)
