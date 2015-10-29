# backend for localization processing

# grid

# localization model
import geopy
import numpy as np
import configparser
import time 


from gps_driver import init_gps, get_location, wait_for_fix
from radio_driver import init_radio, get_rssi

LAT = 0
LON = 1
ERR = 2

ADDR = 0
RSSI = 1

CONFIG_FILENAME = 'grid_config.cfg'

GPS_FREQUENCY = 1 # hz

# given a rssi and tx power, estimate the path length 
# (for more complex models, maybe include frequency, base station height..)

# define some propagation models
# maybe solve for attenuation coefficient, constant amplitude offset, and location
def freespace_dist_pdf(rssi, grid, txpower, error):
    return 0

class measurement():
    # take in a lat/long/err tuple for gps driver
    def __init__(self, location, rssi, addr):
        self.lat = location[LAT]
        self.lon = location[LON]
        self.err = location[ERR]
        self.addr = addr
        self.rssi = rssi


class localizer():
    def __init__(self, gps_port, radio_port):
        self.gps = init_gps(gps_port)
        self.radio = init_radio(radio_port)
        self.node_measurements = {}
        self.location_time = 0

        # read in grid_config.cfg
        # create localization grid
        # if center is auto,
        # read in current location, center grid
    
    def wait_for_gps_fix(self):
        wait_for_fix(self.gps)
        self.get_location()
    
    # frequency of incoming rssi packets is far higher than the gps sampling rate
    # so, return an old gps value unless we are expecting a new one
    # a better way of doing this would maybe to make get_location non-blocking, and only update if it is succesful?
    def update_location(self):
        if self.location_time - time.time() > GPS_FREQUENCY:
            self.location = get_location(self.gps)
            self.location_time = time.time()

    # listen over the radio for another packet
    # record the rssi, gps error, and current location
    def add_measurement(self): 
        self.update_location()
        rssi_packet = get_rssi(self.radio)
        
        addr = rssi_packet[ADDR]
        rssi = rssi_packet[RSSI]

        if not addr in self.node_measurements:
            self.node_measurements[addr] = []

        self.node_measurements[addr].append(measurement(self.location, rssi, addr))
   
    def process_location_map(self):
        # process current measurements into a new map
        # use bayes approach to solve for location and transmit power
        # using sets of location and rssi
        pass

    def export_location_map(self, filename):
        # export location map as kml
        pass

    def print_location_map(self):
        # dump location map to terminal
        pass


def main():
    gps_port = '/dev/ttyUSB0'
    radio_port = '/dev/ttyUSB1'

    loc = localizer(gps_port, radio_port)

    loc.wait_for_gps_fix() 

    try:
        while(True):
            loc.add_measurement()
    
    loc.process_location_map()
    loc.print_location_map()
    loc.export_location_map('loc_map.kml')



if __name__ == '__main__()':
    main()

