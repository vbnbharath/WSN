# driver for communcating with sensor nodes
# stick node in rx mode and listen for packets
# for each packet, capture the addr and rssi
# build up table passively
# capture set of (addr, rssi, loc, location error) on microcontroller
# 4, 4, 1, 1, 1. 11 bytes per record
# dedicate 8 kilobytes..
# okay, but we probably don't have 8 bytes of entropy in a GPS location
import serial

def init_radio():
    radio_serial = None

    return radio_serial 

def get_rssi():
    # wait for rssi packet from radio..
    return rssi


