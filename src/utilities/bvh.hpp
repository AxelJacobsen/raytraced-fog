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

class BVH {
public:
    std::vector<BVHNode> nodes;
    std::vector<Triangle> triangles;

    int build(std::vector<Triangle>& input) {
        triangles = input;
        nodes.reserve(triangles.size() * 2);
        return buildNode(0, triangles.size());
    }

private:
    static const int BIN_COUNT = 16;
    static const int LEAF_SIZE = 4;

    int buildNode(int start, int count) {
        BVHNode node;
        AABB bounds;

        for (int i = start; i < start + count; i++)
            bounds.expand(triangles[i].bounds);

        node.bounds = bounds;

        int nodeIndex = nodes.size();
        nodes.push_back(node);

        if (count <= LEAF_SIZE) {
            nodes[nodeIndex].firstTri = start;
            nodes[nodeIndex].triCount = count;
            return nodeIndex;
        }

        // Choose axis
        glm::vec3 extent = bounds.max - bounds.min;
        int axis = (extent.x > extent.y && extent.x > extent.z) ? 0 :
            (extent.y > extent.z ? 1 : 2);

        // Binning
        struct Bin { AABB bounds; int count = 0; };
        Bin bins[BIN_COUNT];

        float minC = bounds.min[axis];
        float maxC = bounds.max[axis];
        float scale = BIN_COUNT / (maxC - minC + 1e-5f);

        for (int i = start; i < start + count; i++) {
            int b = glm::clamp(int((triangles[i].centroid[axis] - minC) * scale), 0, BIN_COUNT - 1);
            bins[b].count++;
            bins[b].bounds.expand(triangles[i].bounds);
        }

        // Prefix/suffix
        AABB leftBounds[BIN_COUNT];
        AABB rightBounds[BIN_COUNT];
        int leftCount[BIN_COUNT]{};
        int rightCount[BIN_COUNT]{};

        AABB b;
        int c = 0;
        for (int i = 0; i < BIN_COUNT; i++) {
            b.expand(bins[i].bounds);
            c += bins[i].count;
            leftBounds[i] = b;
            leftCount[i] = c;
        }

        b = AABB{};
        c = 0;
        for (int i = BIN_COUNT - 1; i >= 0; i--) {
            b.expand(bins[i].bounds);
            c += bins[i].count;
            rightBounds[i] = b;
            rightCount[i] = c;
        }

        // Find best split
        float bestCost = FLT_MAX;
        int bestSplit = -1;

        for (int i = 0; i < BIN_COUNT - 1; i++) {
            float cost =
                leftBounds[i].surfaceArea() * leftCount[i] +
                rightBounds[i + 1].surfaceArea() * rightCount[i + 1];

            if (cost < bestCost) {
                bestCost = cost;
                bestSplit = i;
            }
        }

        float splitPos = minC + (bestSplit + 1) / (float)BIN_COUNT * (maxC - minC);

        // Partition
        int mid = start;
        for (int i = start; i < start + count; i++) {
            if (triangles[i].centroid[axis] < splitPos) {
                std::swap(triangles[i], triangles[mid]);
                mid++;
            }
        }

        if (mid == start || mid == start + count) {
            // fallback
            mid = start + count / 2;
        }

        int left = buildNode(start, mid - start);
        int right = buildNode(mid, count - (mid - start));

        nodes[nodeIndex].left = left;
        nodes[nodeIndex].right = right;

        return nodeIndex;
    }
};

#endif