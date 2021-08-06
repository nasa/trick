/* Adapted from Paul Bourke's "Polygonising a scalar field" */ 

#include "marching_cubes.h"
#include "mc_tables.h"

glm::vec4 vlerp(float isoLevel, glm::vec4 vectorA, glm::vec4 vectorB, float valueA, float valueB) {
    // TODO: debug vlerp
    float t = (isoLevel - std::min(valueA, valueB)) / std::abs(valueA - valueB);
    t = 0.5;

    return vectorA * t + vectorB * (1 - t);
}

void generateCellMesh(GridCell &grid, double isoLevel, std::vector<glm::uvec3> &triangles, std::vector<glm::vec4> &vertices)
{
    int geometryCaseIndex = 0;
    int startTriangleIdx = vertices.size();
    for (int i = 0; i < 8; i++) {
        if (grid.isoValues[i] > isoLevel) {
            geometryCaseIndex |= (1 << i);
        }
    }

    // no edges in grid cell contain a triangle vertex
    if(edgeTable[geometryCaseIndex] == 0)
        return;
    
    static const int EDGE_CONNECTIONS[12][2] = {{0,1}, {1,2}, {2,3}, {3, 0}, {4,5}, {5,6}, {6,7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    glm::vec4 origin = glm::vec4(0, 0, 0, 1);
    // On edges that contain a triangle vertex, interpolate that triangle vertex based on the isovalues of the endpoint vertices of that edge
    for (int i = 0; i < 12; i++) {
        if (edgeTable[geometryCaseIndex] & (1 << i)) {
            int vertexOne = EDGE_CONNECTIONS[i][0];
            int vertexTwo = EDGE_CONNECTIONS[i][1];
            vertices.push_back(vlerp(isoLevel, grid.vertices[vertexOne], grid.vertices[vertexTwo], grid.isoValues[vertexOne], grid.isoValues[vertexTwo]));
        } else {
            vertices.push_back(origin);
        }
    }
    
    // Add triangle indices to triangles
    
    for(int i = 0; triTable[geometryCaseIndex][i] != -1; i += 3) {
        int idxOne = triTable[geometryCaseIndex][i];
        int idxTwo = triTable[geometryCaseIndex][i+1];
        int idxThree = triTable[geometryCaseIndex][i+2];
        triangles.push_back(glm::uvec3(idxOne + startTriangleIdx, idxTwo + startTriangleIdx, idxThree + startTriangleIdx));
    }


}
