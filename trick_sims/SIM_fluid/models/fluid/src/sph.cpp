#include "sph.h"
#include <cmath>


//std::vector<Particle> particles;
//std::unordered_map<int, std::vector<Particle>> spatial_grid;


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

