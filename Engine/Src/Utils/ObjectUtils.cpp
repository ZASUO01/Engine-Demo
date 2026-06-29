#include "Engine/Utils/ObjectUtils.h"
#include <SDL2/SDL_log.h>
#define TINYOBJ_NO_EXTERN_VAR
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <functional>

struct VertexHashFunction {
    size_t operator()(const tinyobj::index_t& index) const {
        return(std::hash<int>()(index.vertex_index) ^
               std::hash<int>()(index.normal_index) << 1) >> 1 ^
                std::hash<int>()(index.texcoord_index) << 1;
    }
};

struct VertexEqualFunction {
    bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const {
        return a.vertex_index == b.vertex_index &&
                a.normal_index == b.normal_index &&
                a.texcoord_index == b.texcoord_index;
    }
};


bool ObjectUtils::LoadFromFile(const std::string &fileName, std::vector<float> &outVertices, std::vector<unsigned int> &outIndices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str())) {
        SDL_Log("[OBJECT UTILS] tinyobjloader failed to read %s", fileName.c_str());
        return false;
    }

    if (!warn.empty()) {
        SDL_Log("[OBJECT UTILS] tinyobjloader warning %s", warn.c_str());
    }

    outVertices.clear();
    outIndices.clear();

    std::unordered_map<tinyobj::index_t, unsigned int, VertexHashFunction, VertexEqualFunction> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {

            if (!uniqueVertices.contains(index)) {
                const unsigned int newUniqueIndex = uniqueVertices.size();
                uniqueVertices[index] = newUniqueIndex;

                // x y z positions
                outVertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                outVertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                outVertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                // nx ny nz with fallback to 0
                if (index.normal_index >= 0) {
                    outVertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                    outVertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                    outVertices.push_back(attrib.normals[3 * index.normal_index + 2]);
                } else {
                    outVertices.push_back(0.0f); outVertices.push_back(0.0f); outVertices.push_back(0.0f);
                }

                // u v with reversed coords and fallback to 0
                if (index.texcoord_index >= 0) {
                    outVertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                    outVertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
                } else {
                    outVertices.push_back(0.0f); outVertices.push_back(0.0f);
                }
            }
            outIndices.push_back(uniqueVertices[index]);
        }
    }

    outVertices.shrink_to_fit();
    outIndices.shrink_to_fit();

    return true;
}
