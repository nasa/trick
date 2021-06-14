/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
LIBRARY DEPENDENCY:
	((Fluid.o)
	 (Particle.o))
*******************************************************************************/
#include "../include/Fluid.hh"


int Fluid::default_data() {
	
	for (int i = 0; i < EDGE_NUM_PARTICLES; i++) {
		for (int j = 0; j < EDGE_NUM_PARTICLES; j++) {
			for (int k = 0; k < PARTICLE_DEPTH; k++) {
				particles.push_back(Particle(PARTICLE_DIST * i, PARTICLE_DIST * j, PARTICLE_DIST * k));
			}
		}
	}
	particlesArr = &particles[0];
	
	return 0;
}

int Fluid::update_SPH() {
	int p_start = 0;
	int p_end = particles.size();
	verletUpdatePosition(p_start, p_end);
	computeDensityAndPressure(p_start, p_end);
	computeForces(p_start, p_end);
	timeIntegration(p_start, p_end);
	return 0;

}





void Fluid::computeDensityAndPressure(int p_start, int p_end) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.rho = 0;

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


void Fluid::computeForces(int p_start, int p_end) {

	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		float pressure_force[3] = {0, 0, 0};
		float viscosity_force[3] = {0, 0, 0};

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
		/*
		if (isnan(pi.force[0]) || isnan(pi.force[1]) || isnan(pi.force[2])) {
			pi.force[0] = gravity_force[0];
			pi.force[1] = gravity_force[1];
			pi.force[2] = gravity_force[2];
		}
		*/
	}
	
}

void Fluid::verletUpdatePosition(int p_start, int p_end) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		//pi.pos += DT * pi.velocity;
	}
}
// Velocity Verlet time integrator
void Fluid::timeIntegration(int p_start, int p_end) {
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



std::vector<float> Fluid::getParticlePositions() {
	std::vector<float> positions;
	for (auto& pi : particles) {
		positions.push_back(pi.pos[0]);
		positions.push_back(pi.pos[1]);
		positions.push_back(pi.pos[2]);
	}
	return positions;
}

