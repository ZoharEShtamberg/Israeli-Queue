CC=gcc-12
OBJS=IsraeliQueue.o tool/HackerEnrollment.o tool/main.o tool/FileHelper.o
EXEC=prog
DEBUG=-g
CFLAGS=-std=c99 -Wall -Werror -pedantic-errors -Itool -I. -lm

$(EXEC) : $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG) $(OBJS) -o $@


IsraeliQueue.o: IsraeliQueue.c IsraeliQueue.h
	$(CC) -c $(DEBUG) $(CFLAGS) $*.c
FileHelper.o: tool/FileHelper.c tool/FileHelper.h
	$(CC) -c $(DEBUG) $(CFLAGS) $*.c
HackerEnrollment.o: tool/HackerEnrollment.c IsraeliQueue.h tool/FileHelper.h
	$(CC) -c $(DEBUG) $(CFLAGS) $*.c
main.o: tool/main.c tool/HackerEnrollment.h
	$(CC) -c $(DEBUG) $(CFLAGS) $*.c
clean:
	rm -f $(OBJS) $(EXEC)

