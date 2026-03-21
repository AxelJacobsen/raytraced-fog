#pragma once

// System Headers
#include <glad/glad.h>

// Standard headers
#include <string>

// Constants
const int         windowWidth = 1000;
const int         windowHeight = 1000;
const std::string windowTitle = "Raytraced_fog";
const GLint       windowResizable = GL_FALSE;
const int         windowSamples = 4;

struct CommandLineOptions {
    bool enableMusic;
    bool enableAutoplay;
};