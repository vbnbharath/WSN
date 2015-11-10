# localization using markov chain monte carlo

import emcee
import numpy as np
import pdb

from scipy.spatial import distance

MIN_LOC = 0
MAX_LOC = 1 

def log_prior(target_tuple):
    x, y, sigma = target_tuple 
    
    if sigma < 0:
        return -np.inf
    elif x < MIN_LOC or y < MIN_LOC:
        return -np.inf
    elif x > MAX_LOC or y > MAX_LOC:
        return -np.inf
    else:
        return np.log(1.0 / (MAX_LOC ** 2))

def log_likelihood(target_tuple, rssi_tuple):
    x_target, y_target, sigma = target_tuple 
    x_meas, y_meas, rssi = rssi_tuple
    pdb.set_trace()
    dist = distance.euclidean((x_target, y_target), (x_meas, y_meas))
    
    return np.log(calc_p_rssi(rssi, dist, sigma))

def log_posterior(target_tuple, x, y, rssi):
    rssi_tuple = (x, y, rssi)
    return log_prior(target_tuple) + log_likelihood(target_tuple, rssi_tuple)

# calculates the probability of a rssi given distance and noise
# assuming freespace propagation model
def calc_p_rssi(rssi, dist, sigma):
    d0 = 0
    txpower = 0
    d = d0 * 10 ** ((rssi - txpower)/20)
    p = (1 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-(((dist - d)**2)/(2 * sigma ** 2))) # pdf of normal
    return p

# calculates a random rssi given a distance and noise
def calc_random_rssi(dist, sigma):
    d0 = 1
    txpower = 0
    return txpower + 20 * np.log10(dist/d0) + np.random.normal(loc = 0, scale = sigma)

def main():
    # see https://jakevdp.github.io/blog/2014/06/14/frequentism-and-bayesianism-4-bayesian-in-python/
    ndim = 3 # three dimensions in model
    nwalkers = 50 # 50 MCMC walkers
    nburn = 100 # burn-in period
    nsteps = 100 # number of steps..
    nmeas = 20

    np.random.seed(0)
    starting_guesses = np.random.random((nwalkers, ndim))

    # determine target
    x_target_actual = .4
    y_target_actual = .4
    sigma_actual = .1

    # generate measurements
    x_meas = np.random.random(nmeas)
    y_meas = np.random.random(nmeas)
    dist = [distance.euclidean((x_target_actual, y_target_actual), (x_meas[i], y_meas[i])) for i in range(nmeas)]
    rssi_meas = [calc_random_rssi(dist[i], sigma_actual) for i in range(nmeas)]

    sampler = emcee.EnsembleSampler(nwalkers, ndim, log_posterior, args=(x_meas, y_meas, rssi_meas))
    sampler.run_mcmc(starting_guesses, nsteps)

    pdb.set_trace()


if __name__ == '__main__':
    main()
