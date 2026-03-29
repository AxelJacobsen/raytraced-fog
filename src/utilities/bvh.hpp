#ifndef BVH_HPP
#define BVH_HPP
#pragma once

// System headers
#include <glad/glad.h>

// Standard headers
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include "structs.h"
#include "mesh.h"


//Chat supplied most of this
class BVH {
public:
    std::vector<BVHNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<AABB> bounds;

    int build(std::vector<Triangle>& input) {
        triangles = input;
        nodes.clear();
        bounds.clear();
        nodes.reserve(triangles.size() * 2);
        bounds.reserve(triangles.size() * 2);
        return buildNode();
    }

private:
    int buildNode() {
        triangles;
        nodes.clear();
        bounds.clear();

        BVHNode root{};
        root.firstTri = 0;
        root.triCount = triangles.size();

        AABB globalBounds;
        for (const auto& tri : triangles) {
            globalBounds.expand(tri.bounds);
        }

        nodes.push_back(root);
        bounds.push_back(globalBounds);

        return 0;
    }
};


#endif