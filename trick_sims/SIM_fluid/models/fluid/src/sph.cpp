#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include "sph.h"
#include "RigidBody.h"

extern void updateSPH_GPU(std::vector<Particle> &particles);
extern bool rigidActive;

std::vector<Particle> particles;
std::vector<RigidBody> rBodies;
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

void initRigidBody() {
	std::vector<Particle> rParticles;
	
	for (int i = 0; i < EDGE_NUM_RIGID_PARTICLES; i++) {
		for (int j = 0; j < EDGE_NUM_RIGID_PARTICLES; j++) {
			for (int k = 0; k < PARTICLE_HEIGHT; k++) {
				rParticles.push_back(Particle(RIGID_PARTICLE_DIST * i, RIGID_PARTICLE_DIST * j, RIGID_PARTICLE_DIST * k, ParticleType::RigidBody));
			}
		}
	}
	glm::vec3 zero(0, 0, 0);
	rBodies.push_back(RigidBody(rParticles, zero, zero));

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

void computeRigidContribution() {
	for (auto& rBody : rBodies) {
		for (auto& bi : rBody.bodyParticles) {
			float deltaB = 0;
			for (auto& bk : rBody.bodyParticles) {
				glm::vec3 rik = bi.pos - bk.pos;
				float r = glm::length(rik);
				if (r >= 0 && r < H) {
					deltaB += POLY6 * pow(HSQ - r * r, 3);
				}
			}
			bi.psi = REST_DENS / deltaB;
		}
	}
}

void computeForcesOnRigid() {
	for (auto& rBody : rBodies) {
		for (auto& bj : rBody.bodyParticles) {
			glm::vec3 pressure_force(0, 0, 0);
			glm::vec3 viscosity_force(0, 0, 0);
			for (auto& fi : particles) {
				glm::vec3 rij = fi.pos - bj.pos;
				float r = glm::length(rij);
				if (r > 0 && r <= H) {
					pressure_force += MASS * bj.psi * (fi.pressure / (fi.rho * fi.rho)) * SPIKY_GRAD * pow(H - r, 2);
					float nu = (2 * VISC * H * DT) / (fi.rho + bj.rho);
					glm::vec3 vij = fi.velocity - bj.velocity;
					float eps = 0.01f;
					float pi_ij = -nu * fmin(glm::dot(vij, rij), 0) / (glm::length2(rij) + eps * HSQ);
					viscosity_force += MASS * bj.psi * pi_ij * VISC_LAP * (H - r);
				}
			}
			//printf("(%d, %d, %d)\n")
			bj.force = 0.33f * pressure_force + 3.f * viscosity_force;
			//if (isnan(bj.force.x) || isnan(bj.force.y) || isnan(bj.force.z)) {
				//bj.force = glm::vec3(0, -.01, 0);
			//}
		}
	}
}

void computeForcesRigidOnFluid() {
	for (auto& rBody : rBodies) {
		for (auto& bj : rBody.bodyParticles) {
			glm::vec3 pressure_force(0, 0, 0);
			glm::vec3 viscosity_force(0, 0, 0);
			for (auto& fi : particles) {
				glm::vec3 rij = fi.pos - bj.pos;
				float r = glm::length(rij);
				if (r > 0 && r <= H) {
					pressure_force += MASS * bj.psi * (fi.pressure / (fi.rho * fi.rho)) * SPIKY_GRAD * pow(H - r, 2);
					float nu = (2 * VISC * H * DT) / (fi.rho + bj.rho);
					glm::vec3 vij = fi.velocity - bj.velocity;
					float eps = 0.01f;
					float pi_ij = -nu * fmin(glm::dot(vij, rij), 0) / (glm::length2(rij) + eps * HSQ);
					viscosity_force += MASS * bj.psi * pi_ij * VISC_LAP * (H - r);
				}
			}
			bj.force += -pressure_force - viscosity_force;
			//if (isnan(bj.force.x) || isnan(bj.force.y) || isnan(bj.force.z)) {
				//bj.force = glm::vec3(0, -.01, 0);
			//}
		}
	}
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
		if (rigidActive) {
			for (auto& rBody : rBodies) {
				for (auto& bk : rBody.bodyParticles) {
					glm::vec3 rik = bk.pos - pi.pos;
					float r = glm::length(rik);
					if (r >= 0 && r <= H) {
						pi.rho += bk.psi * POLY6 * pow(HSQ - r * r, 3);
					}

				}
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
					pressure_force -= rij_hat * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * pow(H - r, 2);

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
	if (rigidActive) {
		computeForcesRigidOnFluid();
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

void timeIntegrationRigid() {
	
	for (auto& rBody : rBodies) {
		glm::vec3 tForce(0, 0, 0);
		float mass = 0;
		for (auto& bj : rBody.bodyParticles) {
			if (!isnan(bj.force.x) && !isnan(bj.force.y) && !isnan(bj.force.z)) {
				tForce += bj.force;
				mass += bj.psi;
			}
			
		}
		mass = 300000;
		glm::vec3 gravity_force = mass * G;
		float rigid_volume = PARTICLE_DEPTH * EDGE_NUM_RIGID_PARTICLES * EDGE_NUM_RIGID_PARTICLES;
		tForce += gravity_force;
		tForce.z = 0;
		
		glm::vec3 oldc = rBody.c;
		glm::vec3 oldTVel = rBody.tVel;
		rBody.c = oldc + DT * oldTVel;
		rBody.tVel = oldTVel + DT * tForce / mass;
		printf("(%f, %f, %f)\n", tForce.x, tForce.y, tForce.z);

		for (auto& bj : rBody.bodyParticles) {
			bj.pos = bj.pos - oldc + rBody.c;
			//printf("(%f, %f, %f)\n", bj.pos.x, bj.pos.y, bj.pos.z);
		}
		if (rBody.c.z - EPS < -BOUND) {
			rBody.tVel.z *= RIGID_DAMPING;
			rBody.c.z = -BOUND + EPS;
		}

		if (rBody.c.z + EPS > BOUND) {
			rBody.tVel.z *= RIGID_DAMPING;
			rBody.c.z = BOUND - EPS;
		}

		if (rBody.c.y - EPS < -BOUND) {
			rBody.tVel.y *= RIGID_DAMPING;
			rBody.c.y = -BOUND + EPS;
		}

		if (rBody.c.y + EPS > BOUND) {
			rBody.tVel.y *= RIGID_DAMPING;
			rBody.c.y = BOUND - EPS;
		}
		
		if (rBody.c.x - EPS < -BOUND) {
			rBody.tVel.x *= RIGID_DAMPING;
			rBody.c.x = -BOUND + EPS;
		}

		if (rBody.c.x + EPS > BOUND) {
			rBody.tVel.x *= RIGID_DAMPING;
			rBody.c.x = BOUND - EPS;
		}

	}

}

void updateSPH(int p_start, int p_end) {
	//buildSpatialGrid();
	updateSPH_GPU(particles);
	/*
	computeForcesOnRigid();*/
	/*
	verletUpdatePosition(p_start, p_end);
	computeDensityAndPressure(p_start, p_end);
	computeForces(p_start, p_end);
	timeIntegration(p_start, p_end);*/
	
	//spatial_grid.clear();

}

void updateRigid() {
	computeRigidContribution();
	
	computeForcesOnRigid();
	timeIntegrationRigid();
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

std::vector<float> getRigidPositions() {
	std::vector<float> positions;

	for (auto& rBody : rBodies) {
		for (auto& bj : rBody.bodyParticles) {
			positions.push_back(bj.pos.x);
			positions.push_back(bj.pos.y);
			positions.push_back(bj.pos.z);
		}
	}
	return positions;
}