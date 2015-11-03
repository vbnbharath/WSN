# test script for localization approach
# see http://www.stat.missouri.edu/~bayes/Obayes5_webfiles/ToutorialCourses/brunero.pdf

from pylab import *

CELL_SIZE = 2 # meters
GRID_SIZE = 500 # meters
location_grid = np.zeros(GRID_SIZE / CELL_SIZE, GRID_SIZE / CELL_SIZE)
emitter = (250, 250)


MIN_SHADOW = 1
MAX_SHADOW = 5
SHADOW_WIDTH = 16 
SHADOW_RANGE = np.linspace(MIN_SHADOW, MAX_SHADOW, SHADOW_WIDTH)

MIN_PATH = 2
MAX_PATH = 5
PATH_WIDTH = 16 
PATH_RANGE = np.linspace(MIN_PATH, MAX_PATH, PATH_RANGE)

MIN_TXPOWER = -10 # dBm
MAX_TXPOWER = 10 # dBm
TXPOWER_WIDTH = 16
TXPOWER_RANGE = np.linspace(MIN_TXPOWER, MAX_TXPOWER, TXPOWER_WIDTH)


RSSI_MIN = -105 # dBm
RSSI_MAX = -25 # dBm
RSSI_RES = .5 # dB
RSSI_RANGE = np.arange(RSSI_MIN, RSSI_MAX, RSSI_RES)

DIST_MIN = 1 # M
DIST_MAX = ceil(2 * GRID_SIZE ** 2) # M
DIST_WIDTH = 256
DIST_RANGE = np.linspace(DIST_MIN, DIST_MAX, DIST_WIDTH)

# use log normal shadowing propagation model
def log_normal_shadow_path_loss(dist, path_loss_exponent, shadow = finfo(np.double).resolution):
    return 10 * path_loss_exponent * log10(dist) + normal(loc = 0, scale = shadow)

# TODO:$
# so, precompute p(rssi | shadow, path, dist, txpower) table
# dists 
# 128 * 32 * 32 * 16 
# then, for each node
#           for each measurement, compute p(location | rssi, measurement_location)
#           assuming measurements are independent, multiply out (log probabilities?)
#           now we have a grid with p(rssi | measurements, other parameters)
#           examine grid, try to identify location, txpower, and propagation model? 
