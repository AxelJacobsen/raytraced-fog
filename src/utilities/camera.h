#ifndef CAMERA_HPP
#define CAMERA_HPP
#pragma once

// System headers
#include <glad/glad.h>

// Standard headers
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "window.hpp"

namespace Fog
{
    class Camera
    {
    public:
        glm::vec3& pos;
        glm::vec3& direction;
        float focal_length = 1.0;
        
        Camera() : pos(glm::vec3{ 0.0, 0.0, 0.0 }), direction(glm::vec3{ 0.0, 0.0, -1.0 }) {};

    private:
        Camera(Camera const&) = delete;
        Camera& operator = (Camera const&) = delete;
    };
}

#endif
