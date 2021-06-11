#pragma once
#include <vector>
//#include <tuple>
//#include <unordered_map>

const static float G_STRENGTH = 600;
// external graviational force
const static float G[3] = {0.f, G_STRENGTH * -9.8f, 0.f};
const static float REST_DENS = 1000.f;
const static float GAS_CONST = 100.f;
// kernel radius
const static float H = 16.f;
const static float HSQ = H * H;
// assume all particles have the same mass
const static float MASS = 65.f;
// viscosity constant
const static float VISC = 700.f;
// timestep size
const static float DT = 0.001f;

// smoothing kernels and their gradients
const static float POLY6 = 315.f / (64.f * M_PI * pow(H, 9.f));
const static float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
const static float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));

// number of particles on the edge of the square
const static int EDGE_NUM_PARTICLES = 32;
const static int PARTICLE_DEPTH = 4;
const static int NUM_PARTICLES = PARTICLE_DEPTH * EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES;
const static float PARTICLE_DIST = 5;


// boundary parameters 
const static float EPS = H; // boundary epsilon
const static float BOUND_DAMPING = -.9f;

const static float BOUND = 100;

// parameters for spatial grid
/*
const int CELL_SIZE = 2 * H;
const int CELLS_PER_DIM = BOUND / CELL_SIZE;*/



struct Particle {
	Particle(float x_, float y_, float z_) :
		pos {x_, y_, z_},
		velocity {0.f, 0.f, 0.f},
		force {0.f, 0.f, 0.f},
		rho(0),
		pressure(0.f) {}
	float pos[3];
	float velocity[3];
	float force[3];
	float rho;
	float pressure;
};




//void buildSpatialGrid();
void initSPH();
void timeIntegration(int p_start, int p_end);
void computeDensityAndPressure(int p_start, int p_end);
void computeForces(int p_start, int p_end);
void updateSPH(int p_start, int p_end);



//std::vector<Particle> getNeighbors(int grid_x, int grid_y, int grid_z);
std::vector<float> getParticlePositions();