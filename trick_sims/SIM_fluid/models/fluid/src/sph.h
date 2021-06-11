#pragma once
#include <vector>
//#include <tuple>
//#include <unordered_map>



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