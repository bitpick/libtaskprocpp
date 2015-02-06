CC	= g++
CFLAGS	= -Wall -Wextra
CXXFLAGS = $(CFLAGS)
LD	= ld
LN  = ln -s
INSTALL = install
LDFLAGS =

OBJFILES = Mutex.o \
            Task.o \
            Thread.o \
            ThreadEvent.o \
            TaskQueue.o

all: libtaskprocpp.so.1

.cpp.o:
	$(CC) $(CXXFLAGS) -o $@ -c $<

libtaskprocpp.so.1: $(OBJFILES)
	$(LD) $(LDFLAGS) -dynamic -o $@ $^

clean:
	$(RM) $(OBJFILES) libtaskprocpp.so.1

install:
	$(INSTALL) $(OBJFILES) libtaskprocpp.so.1
	$(LN) libtaskprocpp.so.1 libtaskprocpp.so
