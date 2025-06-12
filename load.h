#pragma once
#include <vector>
#include <string>

bool loadObj(const std::string& filename,
    std::vector<float>& vertices,
    std::vector<float>& normals,
    std::vector<float>& colors,
    std::vector<float>& texCoords,
    int& vertexCount);
