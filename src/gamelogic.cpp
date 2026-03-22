#include <chrono>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <utilities/shader.hpp>
#include <utilities/ray.hpp>
#include <utilities/camera.h>
#include <glm/vec3.hpp>
#include <iostream>
#include <SFML/Audio/Sound.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include "gamelogic.h"
#include "utilities/window.hpp"
#include "utilities/timeutils.h"
#include "utilities/structs.h"
#include "utilities/mesh.h"
#include "utilities/bvh.hpp"
#include "utilities/scene.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


Fog::Shader* shader;
Fog::Shader* compShader;
Fog::Camera* camera = new Fog::Camera();

Scene* scene = new Scene();

Mesh* torusMesh = new Mesh();
Mesh* sphereMesh = new Mesh();

const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;
double gameElapsedTime = debug_startTime;

double lastMouseX = windowWidth / 2;
double lastMouseY = windowHeight / 2;

double mouseSensitivity = 1.0;

int image_width = 400;
float aspect_ratio = windowWidth / windowHeight;

GLuint tex;

void mouseCallback(GLFWwindow* window, double x, double y) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

void initGame(GLFWwindow* window) {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    shader = new Fog::Shader();
    shader->makeBasicShader("../res/shaders/simple.vert", "../res/shaders/simple.frag");
    shader->activate();    
    
    compShader = new Fog::Shader();
    compShader->makeFreakShader("../res/shaders/simple.comp");
    compShader->activate();

    if (!torusMesh->loadFromFile("../res/models/torus.fbx")) {
        std::cout << "Error in loading torus" << std::endl;
    };

    if (!sphereMesh->loadFromFile("../res/models/sphere.fbx")) {
        std::cout << "Error in loading sphere" << std::endl;
    };

    scene->meshes.push_back(torusMesh);
    scene->meshes.push_back(sphereMesh);
    scene->build();

    std::vector<Vertex> allVertices;
    std::vector<uint32_t> allIndices;
    std::vector<BVHNode> allNodes;
    std::vector<MeshGPU> meshInfos;

    int vertexOffset = 0;
    int indexOffset = 0;
    int nodeOffset = 0;

    for (int i = 0; i < scene->meshes.size(); i++) {
        allVertices.insert(allVertices.end(), scene->meshes[i]->vertices.begin(), scene->meshes[i]->vertices.end());

        for (auto idx : scene->meshes[i]->indices) {
            allIndices.push_back(idx + vertexOffset);
        }
        allNodes.insert(allNodes.end(), scene->bvhs[i].nodes.begin(), scene->bvhs[i].nodes.end());
        meshInfos.push_back({
            vertexOffset,
            indexOffset,
            static_cast<int>(scene->meshes[i]->indices.size()),
            nodeOffset,
            static_cast<int>(scene->bvhs[i].nodes.size())
            });

        vertexOffset += scene->meshes[i]->vertices.size();
        indexOffset += scene->meshes[i]->indices.size();
        nodeOffset += scene->bvhs[i].nodes.size();
    }

    //Generate scree quad
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0,
        GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //-- Abyssmal dogshit
    // BVH nodes
    GLuint bvhBuffer;
    glGenBuffers(1, &bvhBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        allNodes.size() * sizeof(BVHNode),
        allNodes.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bvhBuffer);

    // Vertex buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        allVertices.size() * sizeof(Vertex),
        allVertices.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexBuffer);

    // Index buffer
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        allIndices.size() * sizeof(uint32_t),
        allIndices.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indexBuffer);

    // Mesh info buffer
    GLuint meshBuffer;
    glGenBuffers(1, &meshBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        meshInfos.size() * sizeof(MeshGPU),
        meshInfos.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, meshBuffer);

}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);

    glm::vec3 cameraPosition = glm::vec3(0, 2, -20);
}

void renderFrame(GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    compute();
    draw();
}

void compute() {
    compShader->activate();
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(
        (GLuint)(windowWidth / 16 + 1),
        (GLuint)(windowHeight / 16 + 1),
        1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    compShader->deactivate();
}

void draw() {
    shader->activate();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shader->get(), "screenTex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader->deactivate();
}