#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Test de la décharge d’une batterie
# python -m SimpleHTTPServer 8000

import time
import serial
import sys
import os

if sys.platform in [ 'darwin' ]:
    port = "/dev/tty.wchusbserial14240"
elif sys.platform in [ 'linux2' ]:
    port = "/dev/ttyUSB0"

RS232 = serial.Serial( port, 115200, timeout=0 )
dataFileName = "data.csv"





def main():
    while True:
        data = RS232.readline()
        if len( data ) > 0:
            utcnow = time.time()
            data = "%s,%s" % ( utcnow, data[ :-1 ] )
            sys.stdout.write( "%s\r" % data )
            sys.stdout.flush()
            with open( dataFileName, "a" ) as dataFile:
                dataFile.write( data )
        else:
            time.sleep( 0.1 )





if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print '\nInterrupted'
        try:
            RS232.close()
            print 'sys.exit(0)'
            sys.exit(0)
        except SystemExit:
            RS232.close()
            print 'os._exit(0)'
            os._exit(0)

