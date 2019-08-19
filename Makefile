

PROGRAM_NAME=tempSensor

ROOTDIR ?= $(abspath ../SDK)

OBJ_LIST = main.o
OBJ_LIST += dht22.o

ifeq (sim, $(notdir $(PLATFORM)))
	OBJ_LIST+=sim.o
endif

include $(ROOTDIR)/terminal/app.mk
