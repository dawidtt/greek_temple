#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <string>

bool loadObj(
    const std::string& filename,
    std::vector<float>& out_vertices,
    std::vector<float>& out_normals,
    std::vector<float>& out_colors,
    std::vector<float>& out_texCoords,  // DODANE
    int& out_vertexCount
) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename)) {
        std::cerr << "Failed to load " << filename << "\n";
        return false;
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            // Wierzcho³ki
            int vi = 3 * index.vertex_index;
            out_vertices.push_back(attrib.vertices[vi + 0]);
            out_vertices.push_back(attrib.vertices[vi + 1]);
            out_vertices.push_back(attrib.vertices[vi + 2]);
            out_vertices.push_back(1.0f);

            // Normale
            if (index.normal_index >= 0) {
                int ni = 3 * index.normal_index;
                out_normals.push_back(attrib.normals[ni + 0]);
                out_normals.push_back(attrib.normals[ni + 1]);
                out_normals.push_back(attrib.normals[ni + 2]);
                out_normals.push_back(0.0f);
            }
            else {
                out_normals.insert(out_normals.end(), { 0.0f, 0.0f, 0.0f, 0.0f });
            }

            // Kolor
            out_colors.insert(out_colors.end(), { 0.7f, 0.7f, 0.7f, 1.0f });

            // Tekstury (UV)
            if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
                int ti = 2 * index.texcoord_index;
                out_texCoords.push_back(attrib.texcoords[ti + 0]);
                out_texCoords.push_back(attrib.texcoords[ti + 1]);
            }
            else {
                out_texCoords.push_back(0.0f);
                out_texCoords.push_back(0.0f);
            }
        }
    }

    out_vertexCount = out_vertices.size() / 4;
    return true;
}
