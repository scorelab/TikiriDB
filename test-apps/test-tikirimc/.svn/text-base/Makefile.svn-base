CONTIKI = /opt/contiki-2.x

ifndef TARGET
TARGET=cooja
endif

ROUTING_DIR = ../../node/routing

PROJECTDIRS += $(ROUTING_DIR)

ifeq ($(TARGET),)
  TARGET=cooja
endif

all: test-*

include $(ROUTING_DIR)/Makefile.routing

include $(CONTIKI)/Makefile.include


