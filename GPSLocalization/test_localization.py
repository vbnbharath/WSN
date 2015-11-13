# test script for localization approach

# see http://www.stat.missouri.edu/~bayes/Obayes5_webfiles/ToutorialCourses/brunero.pdf

from pylab import *
from scipy.spatial import distance
import pdb


def db_to_lin(d):
    return (10 ** (d/10))

def lin_to_db(d):
    return 10 * log10(d)


def dbm_to_watt(d):
    return db_to_lin(d) / 1000
	
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


# calculate the probability of an rssi given propagation model and distance
# txpow, rssi in watts
# path_loss is scaling factor for path loss in log normal shadowing model
# shadow is not done properly, should be scaled on rssi power.... 
def log_p_rssi_calc(rssi, dist, txpow, path_loss_exponent, shadow):
    # first, calculate nominal rssi given dist, txpow, path_loss_exponent
    # rssi_nom is in linear units
    rssi_nom = txpow + path_loss_exponent * 10 * np.log10(dist)
    
    # next, determine ratio of rssi to rssi_nom in dB
    rssi_ratio_db = lin_to_db(rssi_nom / rssi)

    # then, find the probability of that ratio 
    p = (1 / (shadow * np.sqrt(2 * np.pi))) * np.exp(-((rssi_ratio_db**2)/(2 * (shadow ** 2)))) # pdf of normal

    # .. and return the log probability
    return log(p)

# calculates a random rssi given a distance and noise
# dist - distance in meters
# txpow - transmit power in dBm
# path - path loss exponent
# shadow - log shadowing parameter (dB)
def calc_random_rssi(dist, txpow, path, shadow):
    # calculate rssi
    rssi = txpow + path * 10 * np.log10(dist)

    # calculate noise scaling
    if shadow:
        noise = db_to_lin(np.random.normal(loc = 0, scale = shadow))
    else:
        noise = 1
    
    # return rssi scaled by log shadow noise
    return rssi * noise


def main():
    np.random.seed(0)

    # create synthetic target
    x_target_actual = 100
    y_target_actual = 120
    
    txpow_actual = 0 # dBm
    shadow_actual = 4 # linear scale? 
    path_actual = 3 # path loss exponent

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
