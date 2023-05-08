CC=gcc
OBJS=IsraeliQueue.o tool/HackerEnrollment.o tool/main.o tool/FileHelper.o
EXEC=HackEnrollment
DEBUG=-g
CFLAGS= -Wall -Werror -pedantic-errors -std=c99 -Itool -I/new_home/courses/mtm/public/2223b/ex1 -lm

$(EXEC) : $(OBJS)
	$(CC) $(DEBUG) $(OBJS) -o $@ $(CFLAGS)


IsraeliQueue.o: IsraeliQueue.c IsraeliQueue.h
	$(CC) -c $*.c $(DEBUG) $(CFLAGS)
FileHelper.o: tool/FileHelper.c tool/FileHelper.h
	$(CC) -c $*.c $(DEBUG) $(CFLAGS)
HackerEnrollment.o: tool/HackerEnrollment.c tool/FileHelper.h \
 tool/HackerEnrollment.h IsraeliQueue.h
	$(CC) -c $*.c $(DEBUG) $(CFLAGS)
main.o: tool/main.c tool/HackerEnrollment.h
	$(CC) -c $*.c $(DEBUG) $(CFLAGS)
	
clean:
	rm -f $(OBJS) $(EXEC)

#
