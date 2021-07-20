#include "grid_cell.h"
#include "iostream"


void initializeGridCells(std::vector<GridCell>& gridCells, double bound, int amountPerDim) {
	double min = -bound;
	double max = bound;

	double size = 2 * bound / amountPerDim;
	
	for (double x = min; x < max; x += size) {
		for (double y = min; y < max; y += size) {
			for (double z = min; z < max; z += size) {
				GridCell current;
				// vertex ordering convention accoriding to Paul Bourke
				current.vertices[0] = glm::vec4(x, y, z, 1);
				current.vertices[1] = glm::vec4(x +  size, y, z, 1);
				current.vertices[2] = glm::vec4(x + size, y, z + size, 1);
				current.vertices[3] = glm::vec4(x, y, z + size, 1);
				current.vertices[4] = glm::vec4(x, y + size, z, 1);
				current.vertices[5] = glm::vec4(x + size, y + size, z, 1);
				current.vertices[6] = glm::vec4(x + size, y + size, z + size, 1);
				current.vertices[7] = glm::vec4(x, y + size, z + size, 1);
				
				current.isoValues[0] = 0;
				current.isoValues[1] = 0;
				current.isoValues[2] = 0;
				current.isoValues[3] = 0;
				current.isoValues[4] = 0;
				current.isoValues[5] = 0;
				current.isoValues[6] = 0;
				current.isoValues[7] = 0;
                gridCells.push_back(current);
			}
		}
	}
}