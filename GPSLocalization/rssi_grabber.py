# script for logging rssi and gps data for testing localization
# using a pair of MSP430s running the "WirelessTest" energia example
# and TI AIR BoosterPacks
# and a bluetooth GPS (e.g an android phone with the ShareGPS)

import bluetooth
import pynmea2
import sys
import datetime
import serial
import time
import csv 

GPS_ADDR = "BC:F5:AC:74:7C:93" 
GPS_PORT = 2
MIN_TIME = 2 # seconds

BLOCKING = True

# open up serial port for GPS, return pyserial object
def init_gps(port = GPS_PORT, addr = GPS_ADDR):
    socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    socket.connect((addr, port))
    return socket 

def init_radio(port = '/dev/ttyACM1'):
    ser = serial.Serial(port, timeout = 1)
    return ser

# close radio serial
def close_radio(ser):
    ser.close()

# close GPS socket
def close_gps(socket):
    socket.close()

def get_rssi(ser):
    ser.flushInput() # get rid of stale rssi readings
    rssi = None
    while rssi == None:
        line = ser.readline()
        print(line)
        if line[:2] == 'RX':
            rssi = int(line.split(',')[-3]) # dBm?
            break

    return rssi

# read in data from GPS, return location from most recent GPGGA 
def get_location(gps):
    gps_data = gps.read()
    nmea_stream_reader = pynmea2.NMEAStreamReader()
    gps_loc_msg = None

    for msg in streamreader.next(gps_data):
        print(msg)
        if len(str(msg)) > 50 and str(msg)[:6] == '$GPGGA':
            gps_loc_msg = msg
        
    if gps_loc_msg == None:
        return (0, 0, 0)
    
    return (gps_loc_msg.latitude, gps_loc_msg.longitude, gps_loc_msg.horizontal_dil)

   
if __name__ == '__main__':
    # create log file
    csvfilename = 'rssi_location_log_{}.csv'.format(int(time.time))
    csvfile = open(csvfilename, 'wb') 
    csvwriter = csv.writer(csvfile)
    csvwriter.writerow(['time (s)', 'rssi (dBm)', 'longitude (deg)', 'latitude (deg)', 'error (dil)'])

    # init gps and radio
    gps = init_gps()
    msp_serial = init_radio()
    
    # grab a bunch of data
    while True:
        if BLOCKING:
            if 'n' in raw_input('take another measurement? [y]/n: '):
                break
        location = get_location(gps_file)
        rssi = get_rssi(msp_serial)
        time = time.time()

        print('location: ' + str(location))
        print('rssi: ' + str(rssi))

        time.sleep(MIN_TIME) 
        csvwriter.writerow([time, rssi, location[0], location[1], location[2]])
    
    # close up
    csvfile.close()
    close_gps(gps)
    close_radio(msp_serial)

