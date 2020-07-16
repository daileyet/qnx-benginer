#
#	Makefile for QNX native ipc course module
#

# build option variables
DEBUG = -g

# specification of compiler and linker  
CC = qcc
LD = qcc

# specifications for target platform and compiler version
TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntoarmv7le

OUTPUT_DIR = build
SRC = src
# compile and link options
CFLAGS += $(TARGET) $(DEBUG) -w9
LDFLAGS+= $(TARGET) $(DEBUG)

# binaries to be built
BINS = sample1_msg_server sample1_msg_client
BINS += sample2_pulse_server sample2_pulse_client
BINS += sample3_namelookup_client sample3_namelookup_server
BINS += sample4_disconnect_client sample4_disconnect_server
BINS += sample5_unblock_client sample5_unblock_server
BINS += sample6_iov_client sample6_iov_server
BINS += sample7_event_client sample7_event_server
BINS += sample8_shmem_creator sample8_shmem_user


# make target to build all
all: $(BINS)

# make target to clean up object files, binaries and stripped (.tmp) files
clean:
	rm -f *.o $(BINS)  *.tmp

# dependencies
sample1_msg_server.o: sample1_msg_server.c msg_def.h
sample1_msg_client.o: sample1_msg_client.c msg_def.h

sample2_pulse_server.o: sample2_pulse_server.c msg_def.h
sample2_pulse_client.o: sample2_pulse_client.c msg_def.h

sample3_namelookup_client.o: sample3_namelookup_client.c msg_def.h
sample3_namelookup_server.o: sample3_namelookup_server.c msg_def.h

sample4_disconnect_client.o: sample4_disconnect_client.c msg_def.h
sample4_disconnect_server.o: sample4_disconnect_server.c msg_def.h linked_list.h

sample5_unblock_client.o: sample5_unblock_client.c msg_def.h
sample5_unblock_server.o: sample5_unblock_server.c msg_def.h

sample6_iov_client.o: sample6_iov_client.c msg_def.h
sample6_iov_server.o: sample6_iov_server.c msg_def.h

sample7_event_client.o: sample7_event_client.c msg_def.h
sample7_event_server.o: sample7_event_server.c msg_def.h

sample8_shmem_creator.o: sample8_shmem_creator.c shmem.h
sample8_shmem_creator.o: sample8_shmem_creator.c shmem.h