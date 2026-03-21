#ifndef RAY_HPP
#define RAY_HPP
#pragma once

// System headers
#include <glad/glad.h>

// Standard headers
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <glm/vec3.hpp>

namespace Fog
{
    class Ray
    {
    private:
        glm::vec3& origin;
        glm::vec3& direction;

    public:
        Ray() : origin(glm::vec3{ 0.0, 0.0, 0.0 }), direction(glm::vec3{ 0.0, 0.0, -1.0 }) {};
        Ray(glm::vec3& orig, glm::vec3& dir) : origin(orig), direction(dir) {}
        
        glm::vec3 at(float t) const {
            return origin + t * direction;
        }
    private:
        Ray(Ray const&) = delete;
        Ray& operator = (Ray const&) = delete;
    };
}

#endif
