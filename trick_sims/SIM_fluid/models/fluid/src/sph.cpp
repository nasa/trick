#include "sph.h"
#include <cmath>


std::vector<Particle> particles;
//std::unordered_map<int, std::vector<Particle>> spatial_grid;

void initSPH() {
	for (int i = 0; i < EDGE_NUM_PARTICLES; i++) {
		for (int j = 0; j < EDGE_NUM_PARTICLES; j++) {
			for (int k = 0; k < PARTICLE_DEPTH; k++) {
				particles.push_back(Particle(PARTICLE_DIST * i, PARTICLE_DIST * j, PARTICLE_DIST * k, ParticleType::FluidParticle));
			}
		}
	}
}




void computeDensityAndPressure(int p_start, int p_end) {
	//for (auto &pi : particles) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.rho = 0;
		/*
		int grid_x = CELLS_PER_DIM * ((pi.pos.x + BOUND) / (2 * BOUND));
		int grid_y = CELLS_PER_DIM * ((pi.pos.y + BOUND) / (2 * BOUND));
		int grid_z = CELLS_PER_DIM * ((pi.pos.z + BOUND) / (2 * BOUND));
		std::vector<Particle> candidate_neighbors = getNeighbors(grid_x, grid_y, grid_z);*/
		//if (candidate_neighbors.size() != 0)
			//printf("neighbors size: %d\n", candidate_neighbors.size());
		//for (auto& pj : candidate_neighbors) {
		for (auto& pj : particles) {
			float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
			float r = std::sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
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
		float pressure_force[3] = {0, 0, 0};
		float viscosity_force[3] = {0, 0, 0};

		/*int grid_x = CELLS_PER_DIM * ((pi.pos.x + BOUND) / (2 * BOUND));
		int grid_y = CELLS_PER_DIM * ((pi.pos.y + BOUND) / (2 * BOUND));
		int grid_z = CELLS_PER_DIM * ((pi.pos.z + BOUND) / (2 * BOUND));
		std::vector<Particle> candidate_neighbors = getNeighbors(grid_x, grid_y, grid_z);*/
		//printf("force neighbors size: %d\n", candidate_neighbors.size());
		//for (auto& pj : candidate_neighbors) {
		for (auto& pj : particles) {
			if (&pi != &pj) {
				float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
				float r = std::sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
				float rij_hat[3] = {rij[0] / r, rij[1] / r, rij[2] / r};
				if (r > 0 && r <= H) {
					pressure_force[0] -= rij_hat[0] * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * pow(H - r, 2);
					pressure_force[1] -= rij_hat[1] * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * pow(H - r, 2);
					pressure_force[2] -= rij_hat[2] * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * pow(H - r, 2);
					
					viscosity_force[0] += VISC * MASS * ((pj.velocity[0] - pi.velocity[0]) / pj.rho) * VISC_LAP * (H - r);
					viscosity_force[1] += VISC * MASS * ((pj.velocity[1] - pi.velocity[1]) / pj.rho) * VISC_LAP * (H - r);
					viscosity_force[2] += VISC * MASS * ((pj.velocity[2] - pi.velocity[2]) / pj.rho) * VISC_LAP * (H - r);


				}
			}
		}
		float gravity_force[3] = {pi.rho * G[0], pi.rho * G[1], pi.rho * G[2]};
		pi.force[0] = viscosity_force[0] + pressure_force[0] + gravity_force[0];
		pi.force[1] = viscosity_force[1] + pressure_force[1] + gravity_force[1];
		pi.force[2] = viscosity_force[2] + pressure_force[2] + gravity_force[2];
		if (isnan(pi.force[0]) || isnan(pi.force[1]) || isnan(pi.force[2])) {
			pi.force[0] = gravity_force[0];
			pi.force[1] = gravity_force[1];
			pi.force[2] = gravity_force[2];
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
		pi.pos[0] += DT * pi.velocity[0];
		pi.pos[1] += DT * pi.velocity[1];
		pi.pos[2] += DT * pi.velocity[2];
		pi.velocity[0] += DT * pi.force[0] / pi.rho;
		pi.velocity[1] += DT * pi.force[1] / pi.rho;
		pi.velocity[2] += DT * pi.force[2] / pi.rho;
		
		if (pi.pos[2] - EPS < -BOUND) {
			pi.velocity[2] *= BOUND_DAMPING;
			pi.pos[2] = -BOUND + EPS;
		}

		if (pi.pos[2] + EPS > BOUND) {
			pi.velocity[2] *= BOUND_DAMPING;
			pi.pos[2] = BOUND - EPS;
		}

		if (pi.pos[1] - EPS < -BOUND) {
			pi.velocity[1] *= BOUND_DAMPING;
			pi.pos[1] = -BOUND + EPS;
		}

		if (pi.pos[1] + EPS > BOUND) {
			pi.velocity[1] *= BOUND_DAMPING;
			pi.pos[1] = BOUND - EPS;
		}

		if (pi.pos[0] - EPS < -BOUND) {
			pi.velocity[0] *= BOUND_DAMPING;
			pi.pos[0] = -BOUND + EPS;
		}
		if (pi.pos[0] + EPS > BOUND) {
			pi.velocity[0] *= BOUND_DAMPING;
			pi.pos[0] = BOUND - EPS;
		}
	}
}


void updateSPH(int p_start, int p_end) {

	verletUpdatePosition(p_start, p_end);
	computeDensityAndPressure(p_start, p_end);
	computeForces(p_start, p_end);
	timeIntegration(p_start, p_end);

}



std::vector<float> getParticlePositions() {
	std::vector<float> positions;
	for (auto& pi : particles) {
		positions.push_back(pi.pos[0]);
		positions.push_back(pi.pos[1]);
		positions.push_back(pi.pos[2]);
	}
	return positions;
}



/*
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
}*/

