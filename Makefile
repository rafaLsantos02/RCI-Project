CC=gcc
CFLAGS= -Wall 

COR: aux_func.o funcao.o global.o main.o manage_messages.o socket.o table_manage.o udp_message.o
	$(CC) $(CFLAGS) -o COR aux_func.o funcao.o global.o main.o manage_messages.o socket.o table_manage.o udp_message.o

aux_func.o: aux_func.c header.h struct.h
	$(CC) -c $(CFLAGS) aux_func.c

funcao.o: funcao.c header.h struct.h
	$(CC) -c $(CFLAGS) funcao.c

global.o: global.c header.h struct.h
	$(CC) -c $(CFLAGS) global.c

main.o: main.c header.h struct.h
	$(CC) -c $(CFLAGS) main.c

manage_messages.o: manage_messages.c header.h struct.h
	$(CC) -c $(CFLAGS) manage_messages.c

socket.o: socket.c header.h struct.h
	$(CC) -c $(CFLAGS) socket.c

table_manage.o: table_manage.c header.h struct.h
	$(CC) -c $(CFLAGS) table_manage.c

udp_message.o: udp_message.c header.h struct.h
	$(CC) -c $(CFLAGS) udp_message.c

clean::
	rm -f *.o core a.out COR *~

