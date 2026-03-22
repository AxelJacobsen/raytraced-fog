#ifndef STRUCTS_H
#define STRUCTS_H

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec3 min{ FLT_MAX };
    glm::vec3 max{ -FLT_MAX };

    void expand(const glm::vec3& p) {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    void expand(const AABB& b) {
        expand(b.min);
        expand(b.max);
    }

    float surfaceArea() const {
        glm::vec3 e = max - min;
        return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
    }
};

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 centroid;
    AABB bounds;
};

struct BVHNode {
    AABB bounds;

    int left = -1;
    int right = -1;

    int firstTri = 0;
    int triCount = 0;
};

struct MeshGPU {
    int vertexOffset;
    int indexOffset;
    int indexCount;
    int bvhOffset;
    int bvhNodeCount;
};

#endif