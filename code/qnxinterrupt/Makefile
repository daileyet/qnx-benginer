
#
#	Makefile for interrupt project
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntoarmv7le


CFLAGS += $(DEBUG) $(TARGET) -w9
LDFLAGS+= $(DEBUG) $(TARGET)

BINS = sample1_intsimple

all:	$(BINS)

clean:
	rm -f *.o $(BINS) *.tmp

