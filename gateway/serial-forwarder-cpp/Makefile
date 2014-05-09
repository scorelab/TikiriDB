SOURCES = main.cpp Packet.cpp PacketBuffer.cpp BaseComm.cpp SerialComm.cpp \
          TCPComm.cpp

TARGET = sf 
SOURCETDIR = .
OBJECTDIR = obj

#OBJECTS =$(SOURCES:.c=.o)

oname = ${patsubst %.cpp,%.o,${patsubst %.S,%.o,$(1)}}

ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

OBJECTFILES = ${addprefix $(OBJECTDIR)/,${call oname, $(SOURCES)}}

vpath %.cpp $(SOURCETDIR)

CC = g++
CFLAGS =
LDFLAGS = -lpthread 

ifeq ($(DEBUG),1)
CFLAGS += -DDEBUG_EABLE=1
endif

all: $(SOURCES) $(TARGET) 
	
$(TARGET): $(OBJECTFILES)
	$(CC) $(LDFLAGS) $(OBJECTFILES) -o $@

$(OBJECTDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c

clean:
	rm -rf $(OBJECTDIR) $(TARGET)


