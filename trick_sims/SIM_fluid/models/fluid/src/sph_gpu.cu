#include "cuda_runtime.h"
#include "sph_gpu.cuh"


#define NUM_THREADS 1024
bool particlesOnGPU = false;
Particle* d_particles;
int* d_n;


__global__ void computeDensityAndPressureGPU(Particle* particles, int* n) {
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
			glm::vec3 rij = pj.pos - pi.pos;
			float r = glm::length(rij);
			if (r >= 0 && r <= H) {
				pi.rho += MASS * POLY6 * pow(HSQ - r * r, 3.f);
			}
		}
		pi.pressure = GAS_CONST * (pi.rho - REST_DENS);
	}
}

__global__ void computeForcesGPU(Particle* particles, int* n) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;

	//for(auto &pi : particles) {
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		glm::vec3 pressure_force(0, 0, 0);
		glm::vec3 viscosity_force(0, 0, 0);
		//Particle* candidate_neighbors = all_neighbors[i];
		for (int j = 0; j < *n; j++) {
			//Particle& pj = candidate_neighbors[j];
			Particle& pj = particles[j];
			if (&pi != &pj) {
				glm::vec3 rij = pj.pos - pi.pos;
				float r = glm::length(rij);
				glm::vec3 rij_hat = glm::normalize(rij);
				if (r > 0 && r <= H) {
					pressure_force -= rij_hat * MASS * (pi.pressure + pj.pressure) / (2 * pj.rho) * SPIKY_GRAD * pow(H - r, 2.f);

					viscosity_force += VISC * MASS * ((pj.velocity - pi.velocity) / pj.rho) * VISC_LAP * (H - r);


				}
			}
		}
		glm::vec3 G(0.f, G_STRENGTH * -9.8f, 0.f);
		glm::vec3 gravity_force = pi.rho * G;
		pi.force = viscosity_force + pressure_force + gravity_force;
	}

}
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

__global__ void timeIntegrationGPU(Particle* particles, int* n) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		//pi.pos += DT * pi.velocity;
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

void updateSPH_GPU(std::vector<Particle> &particles) {

	if (!particlesOnGPU) {

		int n = NUM_PARTICLES;

		cudaMalloc(&d_particles, NUM_PARTICLES * sizeof(Particle));

		cudaMalloc(&d_n, sizeof(int));

		cudaMemcpy(d_particles, particles.data(), NUM_PARTICLES * sizeof(Particle), cudaMemcpyHostToDevice);
		cudaMemcpy(d_n, &n, sizeof(int), cudaMemcpyHostToDevice);
		particlesOnGPU = true;
	}

	verletUpdatePosition<<<1, NUM_THREADS>>>(d_particles, d_n);

	computeDensityAndPressureGPU << <1, NUM_THREADS >> > (d_particles, d_n);

	computeForcesGPU << <1, NUM_THREADS >> > (d_particles, d_n);
	
	timeIntegrationGPU << <1, NUM_THREADS >> > (d_particles, d_n);

	cudaDeviceSynchronize();

	cudaMemcpy(particles.data(), d_particles, NUM_PARTICLES * sizeof(Particle), cudaMemcpyDeviceToHost);


}

