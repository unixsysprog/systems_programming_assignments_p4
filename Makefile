# 
#	Christopher Fuentes
#	cfuentes@g.harvard.edu
#	HUID 60857326
#
# 	Makefile for Project 4 (pong)
#	Builds pong: a game of the pong variety.
#
#	Formulas:
#	make 		=> builds pong
#

pong: common.h paddle.o pong.o ball.o alarmlib.o
	$(CC) alarmlib.o paddle.o ball.o pong.o -o pong -lncurses 

pong.o: pong.c
	$(CC) -Wall -c pong.c -lncurses 

paddle.o: paddle.h paddle.c
	$(CC) -Wall -c paddle.c -lncurses 

ball.o: ball.h ball.c
	$(CC) -Wall -c ball.c -lncurses 

alarmlib.o: alarmlib.h alarmlib.c
	$(CC) -Wall -c alarmlib.c 


.PHONY: clean
clean:
	rm -f *.o pong
	