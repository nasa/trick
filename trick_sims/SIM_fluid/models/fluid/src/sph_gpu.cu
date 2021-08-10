

#include "cuda_runtime.h"
#include "sph_gpu.h"


#define NUM_THREADS 1024

Particle* d_particles;
Particle** d_spatial_grid;
int* d_cell_counts;
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
		
		for (int j = 0; j < *n; j++) {
			
			//Particle &pj = candidate_neighbors[j];
			
			Particle& pj = particles[j];
			float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
			float r = sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
			if (r >= 0 && r <= fluid->H) {
				pi.rho += fluid->MASS * fluid->POLY6 * pow(fluid->HSQ - r * r, 3.f);
				
			}
		}
/*
		int grid_x = fluid->CELLS_PER_DIM * ((pi.pos[0] + fluid->BOUND) / (2 * fluid->BOUND));
		int grid_y = fluid->CELLS_PER_DIM * ((pi.pos[1] + fluid->BOUND) / (2 * fluid->BOUND));
		int grid_z = fluid->CELLS_PER_DIM * ((pi.pos[2] + fluid->BOUND) / (2 * fluid->BOUND));

		
		
		for (int x = grid_x - 1; x <= grid_x + 1; x++) {
			for (int y = grid_y - 1; y <= grid_y + 1; y++) {
				for (int z = grid_z - 1; z <= grid_z + 1; z++) {
					
					if (x >= 0 && x < fluid->CELLS_PER_DIM && y >= 0 && y < fluid->CELLS_PER_DIM && z >= 0 && z < fluid->CELLS_PER_DIM) {
						int grid_idx = x + y * fluid->CELLS_PER_DIM + z * fluid->CELLS_PER_DIM * fluid->CELLS_PER_DIM;
						int num_neighbors = cell_counts[grid_idx];
						if (num_neighbors == 0) {
							continue;
						}
						Particle* candidate_neighbors = spatial_grid[grid_idx];
						
						
						for (int j = 0; j < num_neighbors; j++) {
							
							Particle &pj = candidate_neighbors[j];
							
							//Particle& pj = particles[j];
							float rij[3] = {pj.pos[0] - pi.pos[0], pj.pos[1] - pi.pos[1], pj.pos[2] - pi.pos[2]};
							float r = sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]);
							if (r >= 0 && r <= fluid->H) {
								pi.rho += fluid->MASS * fluid->POLY6 * pow(fluid->HSQ - r * r, 3.f);
								
							}
						}
						
					}
				}
			}
		}
*/

		pi.pressure = fluid->GAS_CONST * (pi.rho - fluid->REST_DENS);
	}

	if (*n % NUM_THREADS != 0 && tid < (*n - NUM_THREADS * block_size)) {
		int leftover_start = (*n / NUM_THREADS) * NUM_THREADS;
		p_start = tid + leftover_start;
		p_end = (tid + 1) + leftover_start;

		for (int i = p_start; i < p_end; i++) {
			Particle& pi = particles[i];
			pi.rho = 0;
			
			for (int j = 0; j < *n; j++) {
				
				//Particle &pj = candidate_neighbors[j];
				
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

	if (*n % NUM_THREADS != 0 && tid < (*n - NUM_THREADS * block_size)) {
		int leftover_start = (*n / NUM_THREADS) * NUM_THREADS;
		p_start = tid + leftover_start;
		p_end = (tid + 1) + leftover_start;

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
}

__global__ void verletUpdatePosition(Particle* particles, int* n, Fluid* fluid) {
	int tid = threadIdx.x;
	// assuming n is a multiple of NUM_THREADS
	int block_size = *n / NUM_THREADS;
	int p_start = tid * block_size;
	int p_end = (tid + 1) * block_size;
	for (int i = p_start; i < p_end; i++) {
		Particle& pi = particles[i];
		pi.pos[0] += fluid->DT * pi.velocity[0];
		pi.pos[1] += fluid->DT * pi.velocity[1];
		pi.pos[2] += fluid->DT * pi.velocity[2];
	}

	if (*n % NUM_THREADS != 0 && tid < (*n - NUM_THREADS * block_size)) {
		int leftover_start = (*n / NUM_THREADS) * NUM_THREADS;
		p_start = tid + leftover_start;
		p_end = (tid + 1) + leftover_start;
		for (int i = p_start; i < p_end; i++) {
			Particle& pi = particles[i];
			pi.pos[0] += fluid->DT * pi.velocity[0];
			pi.pos[1] += fluid->DT * pi.velocity[1];
			pi.pos[2] += fluid->DT * pi.velocity[2];
		}
	}
	
}

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

	if (*n % NUM_THREADS != 0 && tid < (*n - NUM_THREADS * block_size)) {
		int leftover_start = (*n / NUM_THREADS) * NUM_THREADS;
		p_start = tid + leftover_start;
		p_end = (tid + 1) + leftover_start;

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
}

void initSPH_GPU(std::vector<Particle>& particles, Fluid* fluid) {
	int n = fluid->NUM_PARTICLES;
	
	cudaMalloc(&d_particles, n * sizeof(Particle));
	cudaMalloc(&d_fluid, sizeof(Fluid));
	cudaMalloc(&d_n, sizeof(int));

	cudaMemcpy(d_fluid, fluid, sizeof(Fluid), cudaMemcpyHostToDevice);
	cudaMemcpy(d_particles, particles.data(), n * sizeof(Particle), cudaMemcpyHostToDevice);
	cudaMemcpy(d_n, &n, sizeof(int), cudaMemcpyHostToDevice);
		

	/*
		int num_cells = std::pow(fluid->CELLS_PER_DIM, 3);
		cudaMallocManaged(&d_spatial_grid, num_cells * sizeof(Particle*));
		cudaMallocManaged(&d_cell_counts, num_cells * sizeof(int));
		
		for (int i = 0; i < num_cells; i++) 
		{
			if (fluid->spatialGrid.find(i) != fluid->spatialGrid.end()) {
				
				std::vector<Particle> cellParticles = fluid->spatialGrid[i];
				
				Particle* array;
				cudaMallocManaged(&array, cellParticles.size() * sizeof(Particle));
				d_cell_counts[i] = cellParticles.size();

				for(int j = 0; j < cellParticles.size(); j++) {
					array[j] = cellParticles[j];
				}


				d_spatial_grid[i] = array;
			} else {
				cudaMalloc(&d_spatial_grid[i], sizeof(Particle));
				d_cell_counts[i] = n;
			}
		}
	} else {
		for (int i = 0; i < num_cells; i++) 
		{
			if (fluid->spatialGrid.find(i) != fluid->spatialGrid.end()) {
				
				std::vector<Particle> cellParticles = fluid->spatialGrid[i];
				
				Particle* array;
				cudaMallocManaged(&array, cellParticles.size() * sizeof(Particle));
				d_cell_counts[i] = cellParticles.size();

				for(int j = 0; j < cellParticles.size(); j++) {
					array[j] = cellParticles[j];
				}

				cudaFree(d_spatial_grid[i]);
				d_spatial_grid[i] = array;
			} else {
				d_cell_counts[i] = n;
			}
		}*/
	
}

void updateSPH_GPU(std::vector<Particle>& particles, Fluid* fluid) {
	int n = fluid->NUM_PARTICLES;

	verletUpdatePosition<<<1, NUM_THREADS>>>(d_particles, d_n, d_fluid);

	computeDensityAndPressureGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);
	
	computeForcesGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);
	
	timeIntegrationGPU << <1, NUM_THREADS >> > (d_particles, d_n, d_fluid);

	cudaDeviceSynchronize();

	cudaMemcpy(particles.data(), d_particles, n * sizeof(Particle), cudaMemcpyDeviceToHost);
/*
	for (int i = 0; i < num_cells; i++) 
	{
		cudaFree(&d_spatial_grid[i]);
			
	}
*/

	
	

}

void shutdownSPH_GPU(std::vector<Particle>& particles, Fluid* fluid) {
	cudaFree(d_particles);
	cudaFree(d_fluid);
	cudaFree(d_n);
}
