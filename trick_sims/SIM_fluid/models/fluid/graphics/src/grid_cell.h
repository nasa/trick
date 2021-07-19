#pragma once
#include "glm/glm.hpp"

struct GridCell {
    glm::vec4 vertices[8];
    float isoValues[8] = {1, 1, 0, 1, 0, 1, 0, 1};
};