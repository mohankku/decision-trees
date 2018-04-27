CPP = g++
CFLAGS = -g

hicut: hicut.c trie.cpp dheap.c list.c 
	${CPP} ${CFLAGS} -o  hicut $^

all: hicut

clean:
	@rm hicut
