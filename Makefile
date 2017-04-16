OBJS 	= main.o trie.o
SOURCE	= main.c trie.c
HEADER  = trie.h
OUT  	= typing-assistant
CC	= gcc
FLAGS   = -g3 -c -std=c99
# -g3 option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g3 -std=c99 $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

trie.o: trie.c
	$(CC) $(FLAGS) trie.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)