#!/usr/bin/env python3

import RPi.GPIO as GPIO
import smbus

GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_OFF)

i2c = smbus.SMBus(1)
addr = 0x04

def done_cb(channel):
    number = i2c.read_byte(addr)
    print('Received from Arduino:', number)

GPIO.add_event_detect(4, GPIO.FALLING, callback=done_cb)

print('started')
while True:
    number = 0
    try:
        number = int(input('Enter a number between 0 and 255: '))
    except KeyboardInterrupt:
        break
    except:
        print('Invalid number!')
        continue

    if number < 0 or number > 255:
        print('Invalid number!')
        continue
    i2c.write_byte(addr, number)
    print('Data sent to Arduino')

GPIO.cleanup()
