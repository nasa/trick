#pragma once
#include <vector>
#include <stdio.h>
#include "sph.h"

void updateIsoValuesGPUCaller(std::vector<GridCell>& gridCells, std::vector<float> particlePositions, float radius);

