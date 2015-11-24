
import pycuda.driver as cuda
import pycuda.compiler
import pycuda.autoinit

import numpy as np
import pdb


cu_location_grid = '''
#define PI 3.1415

__device__ __constant__ float txpow_lookup[NTXPOWS];
__device__ __constant__ float shadow_lookup[NSHADOWS];
__device__ __constant__ float path_lookup[NPATHS];
__device__ __constant__ float grid_lookup[GRIDWIDTH];
  
__device__ float lin_to_dbm(float d)
{
    return 10 * log10(d * 1000);
}

__device__ float dbm_to_watt(float d)
{
    return pow(d/10, 10) / 1000;
}

__device__ float dbm_rssi_calc(float dist, float txpow, float path)
{
    return dbm_to_watt(lin_to_dbm(txpow) - path * 10 * log10(dist));
}

__device__ float log_p_rssi_calc(float rssi, float dist, float txpow, float path, float shadow)
{
    float rssi_nom = dbm_rssi_calc(dist, txpow, path);
    float rssi_diff_db = rssi_nom - rssi;

    double p = (1 / (shadow * sqrt(2 * PI))) * exp(-(pow(rssi_diff_db, 2))/(2 * pow(shadow, 2))) 
    return logf(p)
}

__device__ float dist_calc(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2))
}

__device__ size_t location_grid_idx(int32_t x_i, int32_t y_i)
{
    return  x_i * GRID_WIDTH * gridDim.x * gridDim.y * blockDim.x + \
            y_i * gridDim.x * gridDim.y * blockDim.x + \
            gridIdx.x * gridDim.y * blockDim.x + \
            gridIdx.y * blockDim.x + \
            threadIdx.x;
}        

// so, call using grid: txpow, path, shadow
// call with block block size of txpow
// grid size of path, shadow
__global__ void calc_location_grid(double *location_grid, int32_t nmeas, float *rssi_meas, float *x_meas, float *y_meas)
{
    double p_xy = 0;

    float txpow = txpow_lookup[threadIdx.x];
    float path = path_lookup[blockIdx.x];
    float shadow = shadow_lookup[blockIdx.y];
   
    for(int32_t x_i = 0; x_i < GRID_WIDTH; x_i++) {
        for(int32_t y_i = 0; y_i < GRID_WIDTH; y_i++) {
            float x = grid_lookup[x_i];
            float y = grid_lookup[y_i];
 
            for(int32_t i = 0; i < nmeas; i++) {
                float dist = dist_calc(x_meas[i], y_meas[i], x, y);
                p_xy += log_p_rssi_calc(rssi_meas[i], dist, txpow, path, shadow); 
            }

            location_grid[location_grid_idx(x_i, y_i)] = p_xy;

        }
    }
}
'''

def cu_build_localization_grid(grid_size, rssi_meas, x_meas, y_meas, txpows, path_losses, shadows):
    # construct 5d (x, y, path, shadow, txpow) grid on GPU
    pass 

def main():
    pass
    # template replace NPATHS, etc..
    # compile source code
    # copy in constants
   
      


if __name__ == '__main__':
    main()
