#pragma once
#include "glm/glm.hpp"
#include <vector>

struct GridCell {
    glm::vec4 vertices[8];
    float isoValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};
};


void updateIsoValues(std::vector<GridCell>& gridCells, std::vector<float> particlePostions, float radius);
void initializeGridCells(std::vector<GridCell>& gridCells, double bound, int amountPerDim);
