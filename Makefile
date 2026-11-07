CC = gcc
TARGET = scsi_sim
OBJS = main.o uart.o nand_sim.o scsi_cdb.o

CFLAGS += -Wall -O2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -static

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) *.o
