SOURCE=ruleFinder.cpp ruleFinder.hpp
EXEC=lcs
CFLAGS= -pipe -O2 -march=native -std=gnu++0x
LIBS=-lpthread
DEBUG=-Wall -g -std=gnu++0x -pipe
##Longest Common Subsequence

all: $(SOURCE)
	g++ $(CFLAGS) $(LIBS) $(SOURCE) -o $(EXEC)

debug: $(SOURCE)
	g++ $(DEBUG) $(LIBS) $(SOURCE) -o $(EXEC)

$(EXEC): $(SOURCE) 
	g++ $(CFLAGS) $(SOURCE) -o $(EXEC)

clean: $(EXEC)
	rm $(EXEC)
