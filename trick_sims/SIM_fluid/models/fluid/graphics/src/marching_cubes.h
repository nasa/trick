#pragma once
#include "grid_cell.h"
#include "glm/glm.hpp"
#include <vector>
void generateCellMesh(GridCell &grid, double isoLevel, std::vector<glm::uvec3> &triangles, std::vector<glm::vec4> &vertices);

