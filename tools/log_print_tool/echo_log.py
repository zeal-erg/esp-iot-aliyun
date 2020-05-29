#encoding:utf-8
#  input parameters
#  1: usb_port;
#  2: baud.
import serial
import sys
import termios
import subprocess

from glob import glob

# print ("脚本名：", sys.argv[0])
# for i in range(1, len(sys.argv)):
#     print ("参数", i, sys.argv[i])


def serial_open():
    real_port = sys.argv[1]
    # print 'port: %s' % real_port
    real_baud = sys.argv[2]
    # print 'baud: %s' % real_baud
    ser = serial.Serial(real_port, real_baud, timeout=0, parity=serial.PARITY_EVEN, rtscts=1)   # open first serial port
    return ser


try:
    ser = serial_open()
    while 2 > 1:
        if(ser.inWaiting() > 0):
            x = ser.read()
            sys.stdout.write(x)

except:
    ser.close()             # close port
    print("safe exit")
