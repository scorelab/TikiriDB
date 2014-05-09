SOURCES = main.c socket.c sf-client.c rimeaddr.c nw-types.c
TARGET = sf-client
SOURCETDIRS = src ../../node/qprocessor
OBJECTDIR = obj

#OBJECTS =$(SOURCES:.c=.o)
INCLUDE_DIRS = . ../../node/qprocessor

oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,$(1)}}

ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

OBJECTFILES = ${addprefix $(OBJECTDIR)/,${call oname, $(SOURCES)}}

INCLUDES = ${addprefix -I,${call oname, $(INCLUDE_DIRS)}}

vpath %.c $(SOURCETDIRS)

CC = gcc
CFLAGS = -g -DGATEWAY
LDFLAGS = -lpthread 

all: $(SOURCES) $(TARGET) 
	
$(TARGET): $(OBJECTFILES)
	$(CC) $(LDFLAGS) $(OBJECTFILES) -o $@

$(OBJECTDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%: %.c

clean:
	rm -rf $(OBJECTDIR) $(TARGET)


