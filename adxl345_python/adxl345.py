#!/usr/bin/env python

import smbus
import time
import sys

bus = smbus.SMBus(1)
address = 0x53

DR_200HZ   = 0x0B

DEVID       = 0x00
OFSX        = 0x1E
OFSY        = 0x1F
OFSZ        = 0x20
DUR         = 0x21
LATENT      = 0x22
WINDOW      = 0x23
INT_ENABLE  = 0x2E
BW_RATE     = 0x2C
POWER_CTL   = 0x2D
DATA_FORMAT = 0x31
DATAX0      = 0x32
DATAX1      = 0x33
DATAY0      = 0x34
DATAY1      = 0x35
DATAZ0      = 0x36
DATAZ1      = 0x37

def readDevId():
    return bus.read_byte_data(address, 0x00)

def setup():
    id = readDevId()
    if id != 0xE5:
        print('Error: device not detected!')
        return False
    # Put in stand by mode
    bus.write_byte_data(address, POWER_CTL, 0x00)
    # OFS, DUR, Latent, Window = 0 as we don't need it
    bus.write_i2c_block_data(address, OFSX, [0x00] * 6)
    # Set power mode and data rate
    bus.write_byte_data(address, BW_RATE, DR_200HZ)
    # Disable interrupts
    bus.write_byte_data(address, INT_ENABLE, 0x00)
    # Set our data format (+- 4g)
    bus.write_byte_data(address, DATA_FORMAT, 0x09)
    # Go into measurement mode
    bus.write_byte_data(address, POWER_CTL, 0x08)
    return True

def shutdown():
    # Go into stand by mode
    bus.write_byte_data(address, POWER_CTL, 0x00)
    return True

# 4mg per LSB
G_UNIT = (0.004)

print('Hello, World!')

setup()
print('Setup complete')

try:
    while True:
        data = bus.read_i2c_block_data(address, DATAX0, 6)
        x = (data[0] << 8) + data[1]
        y = (data[2] << 8) + data[3]
        z = (data[4] << 8) + data[5]
        if x >= 1<<15: x -= 1<<16
        if y >= 1<<15: y -= 1<<16
        if z >= 1<<15: z -= 1<<16
        x *= G_UNIT
        y *= G_UNIT
        z *= G_UNIT
        sys.stdout.write('\r{0:10.4f} {1:10.4f} {2:10.4f}'.format(x, y, z))
        sys.stdout.flush()

except KeyboardInterrupt:
    print('\nInterrupt signal received')

shutdown()
print('Shutdown complete')
