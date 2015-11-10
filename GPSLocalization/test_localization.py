# test script for localization approach

# see http://www.stat.missouri.edu/~bayes/Obayes5_webfiles/ToutorialCourses/brunero.pdf

from pylab import *
from scipy.spatial import distance
import pdb


def dbm_to_watt(d):
    return (10 ** (d/10)) / 1000

	
CELL_SIZE = 2 # meters
GRID_SIZE = 500 # meters
GRID_WIDTH = GRID_SIZE / CELL_SIZE
GRID_RANGE = np.arange(0, GRID_SIZE, CELL_SIZE)

MIN_SHADOW = 1
MAX_SHADOW = 5
SHADOW_WIDTH = 4 
SHADOW_RANGE = np.linspace(MIN_SHADOW, MAX_SHADOW, SHADOW_WIDTH)

MIN_PATH = 2
MAX_PATH = 5
PATH_WIDTH = 4 
PATH_RANGE = np.linspace(MIN_PATH, MAX_PATH, PATH_WIDTH)

MIN_TXPOWER = -10 # dBm
MAX_TXPOWER = 10 # dBm
TXPOWER_WIDTH = 4 
TXPOWER_RANGE = np.linspace(MIN_TXPOWER, MAX_TXPOWER, TXPOWER_WIDTH)
# scale from dBm to watts
TXPOWER_RANGE = dbm_to_watt(TXPOWER_RANGE) 

RSSI_MIN = -105 # dBm
RSSI_MAX = -25 # dBm
RSSI_RES = .5 # dB
RSSI_RANGE = np.arange(RSSI_MIN, RSSI_MAX, RSSI_RES)
RSSI_RANGE = dbm_to_watt(RSSI_RANGE) # convert from dBm to watts
RSSI_WIDTH = len(RSSI_RANGE)

DIST_MIN = 1 # M
DIST_MAX = ceil(2 * GRID_SIZE ** 2) # M
DIST_WIDTH = 128 
DIST_RANGE = np.linspace(DIST_MIN, DIST_MAX, DIST_WIDTH)
DIST_RES = DIST_MAX / DIST_WIDTH

NTARGETS = 1
NMEASUREMENTS = 10

# use log normal shadowing propagation model
def log_normal_shadow_path_loss(dist, path_loss_exponent, shadow = 0):
    shadow = max(shadow, finfo(np.double).resolution)
    return 10 * path_loss_exponent * log10(dist) + normal(loc = 0, scale = shadow)


# estimate memory consumption of the model
def memory_consumption():
    # p(rssi | shadow, path, dist, txpower) table
    rssi_table_bytes = len(DIST_RANGE) * len(RSSI_RANGE) * TXPOWER_WIDTH * PATH_WIDTH * SHADOW_WIDTH * np.float32().nbytes 
    print 'rssi lookup table memory consumption (megabytes): {} '.format(rssi_table_bytes / (1024 * 1024))
    
    # p(locations | rssis, measurement_locations)
    location_bytes = (GRID_WIDTH ** 2) * SHADOW_WIDTH * PATH_WIDTH * TXPOWER_WIDTH
    print 'location grid memory consumption (megabytes): {}'.format(location_bytes / (1024 * 1024))

# calculate the probability of an rssi given propagation model and distance
# txpow, rssi in watts
# path_loss is scaling factor for path loss in log normal shadowing model
# shadow is not done properly, should be scaled on rssi power.... 
def log_p_rssi_calc(rssi, dist, txpow, path_loss, shadow):
    d0 = 0
    d = d0 * 10 ** ((rssi - txpow)/20)
    p = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-(((dist - d)**2)/(2 * sigma ** 2))) # pdf of normal
    return log(p)

# calculates a random rssi given a distance and noise
# dist - distance in meters
# txpow - transmit power in dBm
# path - path loss exponent
# shadow - log shadowing parameter (TODO: scale this properly based on rssi power..)
def calc_random_rssi(dist, txpow, path, shadow):
    return txpower + path * 10 * np.log10(dist) + np.random.normal(loc = 0, scale = shadow)


def main():
    np.random.seed(0)

    # create synthetic target
    x_target_actual = 100
    y_target_actual = 120
    
    txpow_actual = 0 # dBm
    shadow_actual = 4 # linear scale? 
    path_actual = 3 # path loss exponent

    # display memory usage with current parameters
    memory_consumption()

    # lookup table of log probabilities
    # with p(rssi | distance, transmit power, shadowing, and path loss exponent)
    print 'starting computation of p(rssi) lookup table'
    p_rssi_lookup = np.ones((RSSI_WIDTH, DIST_WIDTH, TXPOWER_WIDTH, PATH_WIDTH, SHADOW_WIDTH), dtype = np.float32)
    print 'p_rssi_lookup size: {} mb'.format(p_rssi_lookup.nbytes / 1024 / 1024)
    for (r_i, rssi) in enumerate(RSSI_RANGE):
        #print 'computing for rssi: {}'.format(rssi)
        for (d_i, dist) in enumerate(DIST_RANGE):
            for (t_i, txpow) in enumerate(TXPOWER_RANGE):
                for (p_i, path_loss) in enumerate(PATH_RANGE):
                    for (s_i, shadow) in enumerate(SHADOW_RANGE):
                        p = log_p_rssi_calc(rssi, dist, txpow, path_loss, shadow) 
                        p_rssi_lookup[r_i][d_i][t_i][p_i][s_i] = p 

    # initialize location grid probability map
    # uniform prior, probably not the best assumption
    location_grid = np.ones((GRID_WIDTH, GRID_WIDTH, TXPOWER_WIDTH, PATH_WIDTH, SHADOW_WIDTH), dtype = np.float64)
    print 'location grid size: {} mb'.format(location_grid.nbytes / 1024 / 1024)
    p_prior = np.log(1.0/location_grid.size)
    location_grid = p_prior * location_grid


    for i in range(NMEASUREMENTS):
        print 'computing measurement ' + str(i)
        # create random synthetic measurement
        x_meas = np.random.randint(GRID_WIDTH)
        y_meas = np.random.randint(GRID_WIDTH)
        dist_actual = distance.euclidean((x_meas, y_meas), (x_target_actual, y_target_actual))
        rssi_meas = calc_random_rssi(dist, txpow_actual, path_actual, shadow_actual)
        r_i = np.argmin(np.abs(RSSI_RANGE - rssi_meas))

        for (x_i, x_look) in enumerate(GRID_RANGE):
            for (y_i, y_look) in enumerate(GRID_RANGE):
                dist = distance.euclidean((x_meas, y_meas), (x_look, y_look))
                d_i = np.argmin(np.abs(DIST_RANGE - dist))

                for (t_i, txpow) in enumerate(TXPOWER_RANGE):
                    for (p_i, path_loss) in enumerate(PATH_RANGE):
                        for (s_i, shadow) in enumerate(SHADOW_RANGE):
                            location_grid[x_i][y_i][t_i][p_i][s_i] += p_rssi_lookup[r_i][d_i][t_i][p_i][s_i]

    
    #           now we have a grid with p(rssi | measurements, other parameters)
    #           examine grid, try to identify location, txpower, and propagation model? 


if __name__ =='__main__':
    main()
