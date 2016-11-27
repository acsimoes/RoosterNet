CC=gcc

#Optimisation level
OPT = -O0

#Other Flags
CFLAGS = $(OPT) -Wall -g -std=c++11

#SFML
LIBS =

#Source files
SRCS = RoosterNetClient.c RoosterNetServer.c

#Object files
OBJS = $(SRCS:.cpp=.o)

#Target
TAR = $(SRCS:.cpp=)



#---------------------------------------------------------------------------------
.PHONY: depend clean remove all

.SUFFIXES: .cpp

all: RoosterNetClient RoosterNetServer

RoosterNetClient: RoosterNetClient.o
	$(CC) $(CFLAGS) $(SPATH) RoosterNetClient.o -o RoosterNetClient $(LIBS)

RoosterNetServer: RoosterNetServer.o
	$(CC) $(CFLAGS) $(SPATH) RoosterNetServer.o -o RoosterNetServer $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(SPATH) -c $*.cpp -o $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o
	
clean:
	rm src/*.o $(PROG) test
remove:
	rm 
depend:
	makedepend -- $(CFLAGS) -- $(SRCS)# DO NOT DELETE