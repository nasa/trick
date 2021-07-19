#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/io.hpp"
#include "glm/glm.hpp"
#include "sph.h"



std::vector<Particle> particles;
std::unordered_map<int, std::vector<Particle>> spatial_grid;

void initSPH() {
	for (int i = 0; i < EDGE_NUM_PARTICLES; i++) {
		for (int j = 0; j < EDGE_NUM_PARTICLES; j++) {
			for (int k = 0; k < PARTICLE_DEPTH; k++) {
				particles.push_back(Particle(PARTICLE_DIST * i, PARTICLE_DIST * j, PARTICLE_DIST * k, ParticleType::FluidParticle));
			}
		}
	}
}

void buildSpatialGrid() {
	spatial_grid.clear();
	for (auto& pi : particles) {
		int grid_x = CELLS_PER_DIM * ((pi.pos.x + BOUND) / (2 * BOUND));
		int grid_y = CELLS_PER_DIM * ((pi.pos.y + BOUND) / (2 * BOUND));
		int grid_z = CELLS_PER_DIM * ((pi.pos.z + BOUND) / (2 * BOUND));
		int grid_key = grid_x + grid_y * CELLS_PER_DIM + grid_z * CELLS_PER_DIM * CELLS_PER_DIM;

		if (spatial_grid.find(grid_key) == spatial_grid.end()) {
			std::vector<Particle> single_particle;
			single_particle.push_back(pi);
			spatial_grid[grid_key] = single_particle;
		}
		else {
			spatial_grid[grid_key].push_back(pi);
		}

	}
}

bool checkBounds(int x, int y, int z) {
	return x >= 0 && x < CELLS_PER_DIM&& y >= 0 && y < CELLS_PER_DIM&& z >= 0 && z < CELLS_PER_DIM;
}

std::vector<Particle> getNeighbors(int grid_x, int grid_y, int grid_z) {
	std::vector<Particle> neighbors;
	for (int x = grid_x - 1; x <= grid_x + 1; x++) {
		for (int y = grid_y - 1; y <= grid_y + 1; y++) {
			for (int z = grid_z - 1; z <= grid_z + 1; z++) {
				int grid_key = x + y * CELLS_PER_DIM + z * CELLS_PER_DIM * CELLS_PER_DIM;
				if (checkBounds(x, y, z) && spatial_grid.find(grid_key) != spatial_grid.end()) {
					std::vector<Particle> cell_particles = spatial_grid[grid_key];
					neighbors.insert(neighbors.end(), cell_particles.begin(), cell_particles.end());
				}
			}
		}
	}
	return neighbors;
}


void computeDensityAndPressure(int p_start, int p_end) {
	//for (auto &pi : particles) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.rho = 0;
		int grid_x = CELLS_PER_DIM * ((pi.pos.x + BOUND) / (2 * BOUND));
		int grid_y = CELLS_PER_DIM * ((pi.pos.y + BOUND) / (2 * BOUND));
		int grid_z = CELLS_PER_DIM * ((pi.pos.z + BOUND) / (2 * BOUND));
		std::vector<Particle> candidate_neighbors = getNeighbors(grid_x, grid_y, grid_z);
		//if (candidate_neighbors.size() != 0)
			//printf("neighbors size: %d\n", candidate_neighbors.size());
		for (auto& pj : candidate_neighbors) {
			glm::vec3 rij = pj.pos - pi.pos;
			float r = glm::length(rij);
			if (r >= 0 && r <= H) {
				pi.rho += MASS * POLY6 * pow(HSQ - r * r, 3);
			}

		}

		
		pi.pressure = GAS_CONST * (pi.rho - REST_DENS);
	}
}


void computeForces(int p_start, int p_end) {

	//for(auto &pi : particles) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		glm::vec3 pressure_force(0, 0, 0);
		glm::vec3 viscosity_force(0, 0, 0);

		int grid_x = CELLS_PER_DIM * ((pi.pos.x + BOUND) / (2 * BOUND));
		int grid_y = CELLS_PER_DIM * ((pi.pos.y + BOUND) / (2 * BOUND));
		int grid_z = CELLS_PER_DIM * ((pi.pos.z + BOUND) / (2 * BOUND));
		std::vector<Particle> candidate_neighbors = getNeighbors(grid_x, grid_y, grid_z);
		//printf("force neighbors size: %d\n", candidate_neighbors.size());
		for (auto& pj : candidate_neighbors) {
			//for (int j = p_start; j < p_end; j++) {
				//Particle &pj = particles[j];
			if (&pi != &pj) {
				glm::vec3 rij = pj.pos - pi.pos;
				float r = glm::length(rij);
				glm::vec3 rij_hat = glm::normalize(rij);
				if (r > 0 && r <= H) {
					pressure_force -= rij_hat * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * (float) pow(H - r, 2);

					viscosity_force += VISC * MASS * ((pj.velocity - pi.velocity) / pj.rho) * VISC_LAP * (H - r);


				}
			}
		}
		glm::vec3 gravity_force = pi.rho * G;
		pi.force = viscosity_force + pressure_force + gravity_force;
		
		if (isnan(pi.force.x) || isnan(pi.force.y) || isnan(pi.force.z)) {
			pi.force = gravity_force;
		}
	}
	
}

void verletUpdatePosition(int p_start, int p_end) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		//pi.pos += DT * pi.velocity;
	}
}
// Velocity Verlet time integrator
void timeIntegration(int p_start, int p_end) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.pos += DT * pi.velocity;
		pi.velocity += DT * pi.force / pi.rho;

		if (pi.pos.z - EPS < -BOUND) {
			pi.velocity.z *= BOUND_DAMPING;
			pi.pos.z = -BOUND + EPS;
		}

		if (pi.pos.z + EPS > BOUND) {
			pi.velocity.z *= BOUND_DAMPING;
			pi.pos.z = BOUND - EPS;
		}

		if (pi.pos.y - EPS < -BOUND) {
			pi.velocity.y *= BOUND_DAMPING;
			pi.pos.y = -BOUND + EPS;
		}

		if (pi.pos.y + EPS > BOUND) {
			pi.velocity.y *= BOUND_DAMPING;
			pi.pos.y = BOUND - EPS;
		}

		if (pi.pos.x - EPS < -BOUND) {
			pi.velocity.x *= BOUND_DAMPING;
			pi.pos.x = -BOUND + EPS;
		}
		if (pi.pos.x + EPS > BOUND) {
			pi.velocity.x *= BOUND_DAMPING;
			pi.pos.x = BOUND - EPS;
		}
	}
}

void updateSPH(int p_start, int p_end) {
	buildSpatialGrid();
	
	verletUpdatePosition(p_start, p_end);
	computeDensityAndPressure(p_start, p_end);
	computeForces(p_start, p_end);
	timeIntegration(p_start, p_end);
	
	spatial_grid.clear();
}


std::vector<float> getParticlePositions() {
	std::vector<float> positions;
	for (auto& pi : particles) {
		positions.push_back(pi.pos.x);
		positions.push_back(pi.pos.y);
		positions.push_back(pi.pos.z);
	}
	return positions;
}

