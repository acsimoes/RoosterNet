CC=gcc

#Optimisation level
OPT = -O0

#Other Flags
CFLAGS = $(OPT) -Wall -g

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
	
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o
	
clean:
	rm *.o RoosterNetClient RoosterNetServer
remove:
	rm 
depend:
	makedepend -- $(CFLAGS) -- $(SRCS)# DO NOT DELETE
