# Landon Newberry
# Mechanic's Shop Application
# November 2017

OBJS = queue.o sll.o
OOPTS = -Wall -g -std=c99 -c -pthread
LOPTS = -Wall -g -std=c99


all: clean project

project: $(OBJS) project.o
	gcc $(LOPTS) $(OBJS) -pthread project.o -o project



sll.o: datastructures/sll.c datastructures/sll.h
	gcc $(OOPTS) datastructures/sll.c

queue.o: datastructures/queue.c datastructures/queue.h datastructures/sll.h
	gcc $(OOPTS) datastructures/queue.c

project.o: datastructures/queue.h
	gcc $(OOPTS) project.c



clean:
	rm -f $(OBJS) project.o
	rm -f project
