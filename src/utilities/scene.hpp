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
    std::vector<Light> lights;
    BVH sceneBvh;
    int texId;
    int normalTexId;
    int roughnessTexId;

    void build() {
        bvhs.resize(meshes.size());

        for (size_t i = 0; i < meshes.size(); i++) {
            auto tris = meshes[i]->buildTriangles();
            bvhs[i].build(tris);
            sceneTriangles.insert(sceneTriangles.end(), tris.begin(), tris.end());
        }
        sceneBvh.build(sceneTriangles);
    }
    std::vector<BVHNode> getNodes() {
        std::vector<BVHNode> nodes;
        int offset = 0;
        for (size_t i = 0; i < bvhs.size(); i++) {
            for (size_t o = 0; o < bvhs[i].nodes.size(); o++) {
                BVHNode tempNode;
                tempNode.firstTri = offset;
                tempNode.triCount = bvhs[i].nodes[o].triCount;
                tempNode.texId = meshes[i]->texId;
                offset += tempNode.triCount;
                nodes.push_back(tempNode);
            }
        }
        return nodes;
    }
    void generateTextureArray(int type) {
        std::vector<Texture> allTextures;
        int currentID = 0;


        for (auto& mesh : meshes) {
            if (mesh->textures.empty() || mesh->normals.empty() || mesh->roughness.empty()) {
                mesh->texId = -1;
                continue;
            }
            Texture& tex = mesh->textures[0];
            if (type == 1) {
                tex = mesh->normals[0];
            }            
            else if (type == 2) {
                tex = mesh->roughness[0];
            }

            mesh->texId = currentID;
            allTextures.push_back(tex);

            currentID++;
        }

        if (allTextures.size() <= 0) { return; }

        int layers = (int)allTextures.size();
        int width = allTextures[0].width;
        int height = allTextures[0].height;


        GLuint texArray;
        glGenTextures(1, &texArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            GL_RGBA8,
            width,
            height,
            layers,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        for (int i = 0; i < layers; i++) {
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                0, 0, i,
                width,
                height,
                1,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                allTextures[i].data.data()
            );
        }
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        if (type == 0) {
            texId = texArray;
        }
        else if (type == 1) {
            normalTexId = texArray;
        }        
        else if (type == 2) {
            roughnessTexId = texArray;
        }
    }
};

#endif
