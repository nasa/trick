#include <vector>
#include <tuple>
#include <unordered_map>
#include "lib/glm/glm.hpp"

#define G_STRENGTH 600.f
// external graviational force

#define REST_DENS 1000.f
#define GAS_CONST 100.f
// kernel radius
#define H 16.f
#define HSQ H * H
// assume all particles have the same mass
#define MASS 65.f
// viscosity constant
#define VISC 700.f
// timestep size
#define DT 0.001f

// smoothing kernels and their gradients
#define POLY6 (float) (315.f / (64.f * M_PI * pow(H, 9.f)))
#define SPIKY_GRAD (float) (-45.f / (M_PI * pow(H, 6.f)))
#define VISC_LAP (float) (45.f / (M_PI * pow(H, 6.f)))

// number of particles on the edge of the square
#define EDGE_NUM_PARTICLES 32
#define PARTICLE_DEPTH 1
#define NUM_PARTICLES (PARTICLE_DEPTH * EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES)
#define PARTICLE_DIST 5

// boundary parameters 
#define EPS H // boundary epsilon
#define BOUND_DAMPING -.9f
#define BOUND 100

// parameters for spatial grid
const int CELL_SIZE = 2 * H;
const int CELLS_PER_DIM = BOUND / CELL_SIZE;



void updateSPH_GPU();
std::vector<float> getParticlePositions();