#ifndef SCENE_HPP
#define SCENE_HPP
#pragma once

// System headers
#include <glad/glad.h>

// Standard headers
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "mesh.h"
#include "bvh.hpp"
#include "structs.h"

class Scene {
public:
    std::vector<Mesh*> meshes;
    std::vector<BVH> bvhs;
    std::vector<Triangle> sceneTriangles;
    BVH sceneBvh;

    void build() {
        bvhs.resize(meshes.size());

        for (size_t i = 0; i < meshes.size(); i++) {
            auto tris = meshes[i]->buildTriangles();
            bvhs[i].build(tris);
            sceneTriangles.insert(sceneTriangles.end(), tris.begin(), tris.end());
        }
        sceneBvh.build(sceneTriangles);
    }
};

#endif
