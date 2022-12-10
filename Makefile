.PHONY = all clean
#Defining Macros
CC = gcc
FLAGS = -Wall -g

all: sender receiver
#Creating Program
sender: sender.c
	$(CC) $(FLAGS) -o sender sender.c
receiver: receiver.c
	$(CC) $(FLAGS) -o receiver receiver.c

clean:
	rm -f *.o sender receiver