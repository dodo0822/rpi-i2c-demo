CC=gcc

LD_FLAGS=-lpthread
C_FLAGS=

all: i2c-arduino

i2c-arduino: i2c-arduino.o
	$(CC) $(LD_FLAGS) -o i2c-arduino i2c-arduino.o

%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm *.o i2c-arduino
