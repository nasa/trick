#include "marching_cubes.h"
#include "mc_tables.h"

glm::vec4 vlerp(float isoLevel, glm::vec4 vectorA, glm::vec4 vectorB, float valueA, float valueB) {
    float t = (isoLevel - std::min(valueA, valueB)) / std::abs(valueA - valueB);

    return vectorA * t + vectorB * (1 - t);
}

void generateCellMesh(GridCell &grid, double isoLevel, std::vector<glm::uvec3> &triangles, std::vector<glm::vec4> &vertices)
{
    int geometryCaseIndex = 0;
    if (grid.isoValues[0] < isoLevel)
        geometryCaseIndex |= 1;
    if (grid.isoValues[1] < isoLevel)
        geometryCaseIndex |= 2;
    if (grid.isoValues[2] < isoLevel)
        geometryCaseIndex |= 4;
    if (grid.isoValues[3] < isoLevel)
        geometryCaseIndex |= 8;
    if (grid.isoValues[4] < isoLevel)
        geometryCaseIndex |= 16;
    if (grid.isoValues[5] < isoLevel)
        geometryCaseIndex |= 32;
    if (grid.isoValues[6] < isoLevel)
        geometryCaseIndex |= 64;
    if (grid.isoValues[7] < isoLevel)
        geometryCaseIndex |= 128;

    // no edges in grid cell contain a triangle vertex
    if(edgeTable[geometryCaseIndex] == 0)
        return;
    
    // On edges that contain a triangle vertex, interpolate that triangle vertex based on the isovalues of the endpoint vertices of that edge
    if (edgeTable[geometryCaseIndex] & 1)
        vertices.push_back(vlerp(isoLevel, grid.vertices[0], grid.vertices[1], grid.isoValues[0], grid.isoValues[1]));
    if (edgeTable[geometryCaseIndex] & 2)
        vertices.push_back(vlerp(isoLevel, grid.vertices[1], grid.vertices[2], grid.isoValues[1], grid.isoValues[2]));
    if (edgeTable[geometryCaseIndex] & 4)
        vertices.push_back(vlerp(isoLevel, grid.vertices[2], grid.vertices[3], grid.isoValues[2], grid.isoValues[3]));
    if (edgeTable[geometryCaseIndex] & 8) 
        vertices.push_back(vlerp(isoLevel, grid.vertices[3], grid.vertices[0], grid.isoValues[3], grid.isoValues[0]));
    if (edgeTable[geometryCaseIndex] & 16)
        vertices.push_back(vlerp(isoLevel, grid.vertices[4], grid.vertices[5], grid.isoValues[4], grid.isoValues[5]));
    if (edgeTable[geometryCaseIndex] & 32)
        vertices.push_back(vlerp(isoLevel, grid.vertices[5], grid.vertices[6], grid.isoValues[5], grid.isoValues[6]));
    if (edgeTable[geometryCaseIndex] & 64)
        vertices.push_back(vlerp(isoLevel, grid.vertices[6], grid.vertices[7], grid.isoValues[6], grid.isoValues[7]));
    if (edgeTable[geometryCaseIndex] & 128)
        vertices.push_back(vlerp(isoLevel, grid.vertices[7], grid.vertices[4], grid.isoValues[7], grid.isoValues[4]));
    if (edgeTable[geometryCaseIndex] & 256)
        vertices.push_back(vlerp(isoLevel, grid.vertices[0], grid.vertices[4], grid.isoValues[0], grid.isoValues[4]));
    if (edgeTable[geometryCaseIndex] & 512)
        vertices.push_back(vlerp(isoLevel, grid.vertices[1], grid.vertices[5], grid.isoValues[1], grid.isoValues[5]));
    if (edgeTable[geometryCaseIndex] & 1024)
        vertices.push_back(vlerp(isoLevel, grid.vertices[2], grid.vertices[6], grid.isoValues[2], grid.isoValues[6]));
    if (edgeTable[geometryCaseIndex] & 2048)
        vertices.push_back(vlerp(isoLevel, grid.vertices[3], grid.vertices[7], grid.isoValues[3], grid.isoValues[7]));

    // Add triangle indices to triangles
    int i = 0;
    while (triTable[geometryCaseIndex][i] != -1) {
        int idxOne = triTable[geometryCaseIndex][i];
        int idxTwo = triTable[geometryCaseIndex][i+1];
        int idxThree = triTable[geometryCaseIndex][i+2];
        triangles.push_back(glm::uvec3(idxOne, idxTwo, idxThree));
        i += 3;
    }

}

