SOURCES = main.c socket.c serial.c
TARGET = sf 
SOURCETDIR = src
OBJECTDIR = obj

#OBJECTS =$(SOURCES:.c=.o)

oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,$(1)}}

ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

OBJECTFILES = ${addprefix $(OBJECTDIR)/,${call oname, $(SOURCES)}}

vpath %.c $(SOURCETDIR)

CC = gcc
CFLAGS = 
LDFLAGS = -lpthread 

all: $(SOURCES) $(TARGET) 
	
$(TARGET): $(OBJECTFILES)
	$(CC) $(LDFLAGS) $(OBJECTFILES) -o $@

$(OBJECTDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c

clean:
	rm -rf $(OBJECTDIR) $(TARGET)


