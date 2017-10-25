CC=gcc
LD=gcc
LD_FLAGS=
C_FLAGS=

all: adxl345

adxl345: adxl345.o
	$(LD) $^ $(LD_FLAGS) -o $@

%.o: %.c
	$(CC) $(C_FLAGS) -c $<

.PHONY: clean
clean:
	rm -f adxl345 *.o
    
