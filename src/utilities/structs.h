#ifndef STRUCTS_H
#define STRUCTS_H

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec4 min{ FLT_MAX };
    glm::vec4 max{ -FLT_MAX };

    void expand(const glm::vec4& p) {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    void expand(const AABB& b) {
        expand(b.min);
        expand(b.max);
    }
};

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 centroid;
    AABB bounds;
};

struct BVHNode {
    int firstTri = 0;
    int triCount = 0;
    int texId = -1; 
};

struct MeshGPU {
    int vertexOffset;
    int indexOffset;
    int indexCount;
    int bvhOffset;

    int bvhNodeCount;
    int vertexCount;
    int textId;
    int test;
};

struct AnimTransform {
    glm::vec4 data; // meshid rotSpeed bob speed
    glm::vec4 staticPos; // used for moving + instancing
};


struct Texture {
    int width, height, channels;
    std::vector<unsigned char> data;
    std::string path;
};

struct Light {
    glm::vec4 pos;
    glm::vec4 color;
};

#endif