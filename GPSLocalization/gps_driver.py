# script for communicating with gps
# gps the GPS mt3333 chipset, assuming NMEA output

# TODO: add error handling..


import serial
import pynmea2
import sys

GPS_BAUD = 9600 # baud..
GPS_TIMEOUT = 2 # seconds 

# open up serial port for GPS, return pyserial object
def init_gps(port, baud = GPS_BAUD, timeout = GPS_TIMEOUT):
    gps_serial = serial.Serial(port, baud, timeout)
    return gps_serial

# close GPS serial port
def close_gps(gps_serial):
    if gps_serial.isOpen():
        gps_serial.close()

def wait_for_fix(gps_serial):
    pass

# parse some strings over serial with pynmea and grab the current location 
def get_location(gps_serial):
    assert(gps_serial.isOpen())

    while True:
        gps_line = gps_serial.readline(timeout = GPS_TIMEOUT)
        if gps_line[4] == 'G': # change to GGPGA or whatever..
            if len(gps_line) > 50:
                # change to check on fix quality field..
                gps_loc_msg = pynmea2.parse(gps_line)
                break            

        print("waiting on a properly formed NMEA string from GPS (maybe we don't have a fix yet?)")
        
    return (gps_loc_msg.latitude, gps_loc_msg.longitude, gps_loc_msg.horizontal_dil)


   
    
        

