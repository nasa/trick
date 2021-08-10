
#include <vector>
#include <stdio.h>
#include "../include/Particle.hh"
#include "../include/Fluid.hh"

#ifndef SPH_GPU_H
#define SPH_GPU_H
void initSPH_GPU(std::vector<Particle>& particles, Fluid* fluid);
void updateSPH_GPU(std::vector<Particle>& particles, Fluid* fluid);
void shutdownSPH_GPU(std::vector<Particle>& particles, Fluid* fluid);

#endif
