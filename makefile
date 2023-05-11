CC=gcc
OBJS=IsraeliQueue.o HackEnrollment.o main.o FileHelper.o
EXEC=HackEnrollment
DEBUG=-DNDEBUG
CFLAGS= -Wall -Werror -pedantic-errors -std=c99 -Itool -I/new_home/courses/mtm/public/2223b/ex1 -lm

$(EXEC) : $(OBJS)
	$(CC) $(DEBUG) $(OBJS) -o $@ $(CFLAGS)


IsraeliQueue.o: IsraeliQueue.c IsraeliQueue.h
	$(CC) -c $< $(DEBUG) $(CFLAGS)
FileHelper.o: tool/FileHelper.c tool/FileHelper.h
	$(CC) -c $< $(DEBUG) $(CFLAGS) -o $@
HackEnrollment.o: tool/HackEnrollment.c tool/FileHelper.h tool/HackEnrollment.h IsraeliQueue.h
	$(CC) -c $< $(DEBUG) $(CFLAGS) -o $@
main.o: tool/main.c tool/HackEnrollment.h
	$(CC) -c $< $(DEBUG) $(CFLAGS) -o $@
	
clean:
	rm -f $(OBJS) $(EXEC)

#
