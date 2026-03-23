#pragma once

#include "mesh.h"

unsigned int generateMeshBuffer(Mesh &mesh);
unsigned int generateBuffer(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
