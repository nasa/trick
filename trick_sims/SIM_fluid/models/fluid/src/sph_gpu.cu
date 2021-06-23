

#include "cuda_runtime.h"
#include "sph_gpu.h"


#define NUM_THREADS 1024

bool particlesOnGPU = false;
Particle* d_particles;
Fluid* d_fluid;
int* d_n;


__global__ void computeDensityAndPressureGPU(Particle* particles, int* n, Fluid* fluid) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;

	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.rho = 0;
		//Particle* candidate_neighbors = all_neighbors[i];
		for (int j = 0; j < *n; j++) {
			//Particle & pj = candidate_neighbors[j];
			Particle& pj = particles[j];
			float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
			float r = sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
			if (r >= 0 && r <= fluid->H) {
				pi.rho += fluid->MASS * fluid->POLY6 * pow(fluid->HSQ - r * r, 3.f);
			}
		}
		pi.pressure = fluid->GAS_CONST * (pi.rho - fluid->REST_DENS);
	}
}



__global__ void computeForcesGPU(Particle* particles, int* n, Fluid* fluid) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;

	//for(auto &pi : particles) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		float pressure_force[3] = {0, 0, 0};
		float viscosity_force[3] = {0, 0, 0};
		//Particle* candidate_neighbors = all_neighbors[i];
		for (int j = 0; j < *n; j++) {
			//Particle& pj = candidate_neighbors[j];
			Particle& pj = particles[j];
			if (&pi != &pj) {
				float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
				float r = std::sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
				float rij_hat[3] = {rij[0] / r, rij[1] / r, rij[2] / r};
				if (r > 0 && r <= fluid->H) {
					pressure_force[0] -= rij_hat[0] * fluid->MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * fluid->SPIKY_GRAD * pow(fluid->H - r, 2.f);
					pressure_force[1] -= rij_hat[1] * fluid->MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * fluid->SPIKY_GRAD * pow(fluid->H - r, 2.f);
					pressure_force[2] -= rij_hat[2] * fluid->MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * fluid->SPIKY_GRAD * pow(fluid->H - r, 2.f);

					viscosity_force[0] += fluid->VISC * fluid->MASS * ((pj.velocity[0] - pi.velocity[0]) / pj.rho) * fluid->VISC_LAP * (fluid->H - r);
					viscosity_force[1] += fluid->VISC * fluid->MASS * ((pj.velocity[1] - pi.velocity[1]) / pj.rho) * fluid->VISC_LAP * (fluid->H - r);
					viscosity_force[2] += fluid->VISC * fluid->MASS * ((pj.velocity[2] - pi.velocity[2]) / pj.rho) * fluid->VISC_LAP * (fluid->H - r);


				}
			}
		}
		float G[3] = {0.f, fluid->G_STRENGTH * -9.8f, 0.f};
		float gravity_force[3] = {pi.rho * G[0], pi.rho * G[1], pi.rho * G[2]};
		pi.force[0] = viscosity_force[0] + pressure_force[0] + gravity_force[0];
		pi.force[1] = viscosity_force[1] + pressure_force[1] + gravity_force[1];
		pi.force[2] = viscosity_force[2] + pressure_force[2] + gravity_force[2];
	}

}
/*
__global__ void verletUpdatePosition(Particle* particles, int* n) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.pos += DT * pi.velocity;
	}
}
*/
__global__ void timeIntegrationGPU(Particle* particles, int* n, Fluid* fluid) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		//pi.pos += DT * pi.velocity;
		pi.velocity[0] += fluid->DT * pi.force[0] / pi.rho;
		pi.velocity[1] += fluid->DT * pi.force[1] / pi.rho;
		pi.velocity[2] += fluid->DT * pi.force[2] / pi.rho;

		if (pi.pos[2] - fluid->EPS < -fluid->BOUND) {
			pi.velocity[2] *= fluid->BOUND_DAMPING;
			pi.pos[2] = -fluid->BOUND + fluid->EPS;
		}

		if (pi.pos[2] + fluid->EPS > fluid->BOUND) {
			pi.velocity[2] *= fluid->BOUND_DAMPING;
			pi.pos[2] = fluid->BOUND - fluid->EPS;
		}

		if (pi.pos[1] - fluid->EPS < -fluid->BOUND) {
			pi.velocity[1] *= fluid->BOUND_DAMPING;
			pi.pos[1] = -fluid->BOUND + fluid->EPS;
		}

		if (pi.pos[1] + fluid->EPS > fluid->BOUND) {
			pi.velocity[1] *= fluid->BOUND_DAMPING;
			pi.pos[1] = fluid->BOUND - fluid->EPS;
		}

		if (pi.pos[0] - fluid->EPS < -fluid->BOUND) {
			pi.velocity[0] *= fluid->BOUND_DAMPING;
			pi.pos[0] = -fluid->BOUND + fluid->EPS;
		}
		if (pi.pos[0] + fluid->EPS > fluid->BOUND) {
			pi.velocity[0] *= fluid->BOUND_DAMPING;
			pi.pos[0] = fluid->BOUND - fluid->EPS;
		}
	}

}

void updateSPH_GPU(std::vector<Particle>& particles, Fluid* fluid) {
	int n = fluid->NUM_PARTICLES;
	
	if (!particlesOnGPU) {
		cudaMalloc(&d_particles, n * sizeof(Particle));
		cudaMalloc(&d_fluid, sizeof(Fluid));
		cudaMalloc(&d_n, sizeof(int));

		cudaMemcpy(d_fluid, fluid, sizeof(Fluid), cudaMemcpyHostToDevice);
		cudaMemcpy(d_particles, particles.data(), n * sizeof(Particle), cudaMemcpyHostToDevice);
		cudaMemcpy(d_n, &n, sizeof(int), cudaMemcpyHostToDevice);
		particlesOnGPU = true;
	}

	//verletUpdatePosition<<<1, NUM_THREADS>>>(d_particles, d_n);

	computeDensityAndPressureGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);
	
	computeForcesGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);
	
	timeIntegrationGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);

	cudaDeviceSynchronize();

	cudaMemcpy(particles.data(), d_particles, n * sizeof(Particle), cudaMemcpyDeviceToHost);


}

