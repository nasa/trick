

#include "cuda_runtime.h"

#include "grid_cell.h"
#include <stdio.h>


#define NUM_THREADS 1024



__global__ void updateIsoValuesGPU(GridCell* gridCells, float* particlePositions, int numParticles, int numCells, float radius) {
    int tid = threadIdx.x;

    int blockSize = numCells / NUM_THREADS;

    int g_start = tid * blockSize;
    int g_end = (tid + 1) * blockSize;
    
    for (int i = g_start; i < g_end; i++) {
        for (int j = 0; j < 8; j++) {
            
			gridCells[i].isoValues[j] = 0;

            glm::vec3 vertexPos(gridCells[i].vertices[j]);
            //printf("(%f, %f, %f): %d\n", vertexPos.x, vertexPos.y, vertexPos.z, tid);
            for (int k = 0; k < numParticles; k++) {
                float x = particlePositions[3*k];
                float y = particlePositions[3*k+1];
                float z = particlePositions[3*k+2];
                glm::vec3 particlePos(x, y, z);
                if (glm::length(vertexPos - particlePos) < radius) {
                    gridCells[i].isoValues[j]+=1;
                    
                }
            }
        }
    }
}


void updateIsoValuesGPUCaller(std::vector<GridCell>& gridCells, std::vector<float> particlePositions, float radius) {
	
    GridCell* d_gridCells;
    float* d_particlePositions;

    cudaMalloc(&d_gridCells, gridCells.size() * sizeof(GridCell));
    cudaMalloc(&d_particlePositions, particlePositions.size() * sizeof(float));

    cudaMemcpy(d_gridCells, gridCells.data(), gridCells.size() * sizeof(GridCell), cudaMemcpyHostToDevice);
    cudaMemcpy(d_particlePositions, particlePositions.data(), particlePositions.size() * sizeof(float), cudaMemcpyHostToDevice);
	
	

	updateIsoValuesGPU<<<1, NUM_THREADS>>>(d_gridCells, d_particlePositions, particlePositions.size() / 3, gridCells.size(), radius);


	cudaDeviceSynchronize();

	cudaMemcpy(gridCells.data(), d_gridCells, gridCells.size() * sizeof(GridCell), cudaMemcpyDeviceToHost);

    cudaFree(d_gridCells);
    cudaFree(d_particlePositions);
}
