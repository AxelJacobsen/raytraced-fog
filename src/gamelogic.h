#pragma once

#include <GLFW/glfw3.h>

#include <utilities/window.hpp>

void initGame(GLFWwindow* window);
void updateFrame(GLFWwindow* window);
void renderFrame(GLFWwindow* window);
void compute();
void drawDebug();
void drawComp();
