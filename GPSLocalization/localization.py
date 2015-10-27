# backend for localization processing

# grid

# localization model
from gps_driver import init_gps, get_location
from radio_driver import init_radio, get_rssi
import geopy

class localizer:
    def __init__(self, gps_port = GPS_PORT, radio_port = RADIO_PORT):
        self.gps = init_gps(gps_port)
        self.radio = init_radio(radio_port)
        
        # read in grid_config.cfg
        # create localization grid
        # if center is auto,
        # read in current location, center grid


    def add_current_location(self): 
        location = get_location(self.gps)
        rssis = get_rssi(self.radio)
        self.update_location_map(location, rssis)

    def update_location_map(location, rssis):
        pass
    
    def export_location_map():
        # export location map as kml
        pass

    def print_location_map():
        # dump location map to terminal
        pass
