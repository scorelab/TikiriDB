SOURCES = parser-helper.c socket.c nw-types.c rimeaddr.c tikirisqll.c tikirisqly.c sf-client.c
LEX_SOURCE = tikirisql.l
YACC_SOURCE = tikirisql.y

TARGET = tikirisql
SOURCETDIRS = ../../node/qprocessor
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
LEX = lex -I
YACC = yacc -dv
CFLAGS = -g -DGATEWAY -DYYDEBUG=1
LDFLAGS = -ly -lfl 

all: $(SOURCES) $(TARGET) 
	
$(TARGET): $(OBJECTFILES)
	$(CC) $(OBJECTFILES) -o $@  $(LDFLAGS) 

$(OBJECTDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%: %.c

tikirisqll.c: tikirisqly.c
	$(LEX) -o tikirisqll.c  $(LEX_SOURCE)

tikirisqly.c:
	$(YACC) -o tikirisqly.c $(YACC_SOURCE)

clean:
	rm -rf $(OBJECTDIR) $(TARGET) tikirisqll.c tikirisqly.c tikirisqly.h tikirisqly.output

