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
Fog::Shader* debugShader;
Fog::Camera* camera = new Fog::Camera();

Scene* scene = new Scene();

Mesh* torusMesh = new Mesh();
Mesh* sphereMesh = new Mesh();

std::vector<Vertex> allVertices;
std::vector<uint32_t> allIndices;
std::vector<BVHNode> allNodes;
std::vector<MeshGPU> meshInfos;


const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;
double gameElapsedTime = debug_startTime;

double lastMouseX = windowWidth / 2;
double lastMouseY = windowHeight / 2;

double mouseSensitivity = 1.0;

int image_width = 400;
float aspect_ratio = windowWidth / windowHeight;

bool computeView = true;
bool mouseHeld = false;

GLuint tex;
GLuint debugVao;
GLuint quadVAO;

GLuint modelLoc;
GLuint viewLoc;
GLuint projLoc;

void mouseCallback(GLFWwindow* window, double x, double y) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    camera->handleKeyboardInputs(key, action);
}

void initGame(GLFWwindow* window) {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);


    // Init raytracing shaders and meshes
    shader = new Fog::Shader();
    shader->makeBasicShader("../res/shaders/raytrace.vert", "../res/shaders/raytrace.frag");
    shader->activate();    
    
    compShader = new Fog::Shader();
    compShader->makeFreakShader("../res/shaders/raytrace.comp");
    compShader->activate();

    debugShader = new Fog::Shader();
    debugShader->makeBasicShader("../res/shaders/debug.vert", "../res/shaders/debug.frag");

    if (!torusMesh->loadFromFile("../res/models/torus.fbx")) {
        std::cout << "Error in loading torus" << std::endl;
    };

    if (!sphereMesh->loadFromFile("../res/models/sphere.fbx")) {
        std::cout << "Error in loading sphere" << std::endl;
    };

    scene->meshes.push_back(torusMesh);
    scene->meshes.push_back(sphereMesh);
    scene->build();

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

    // -------------------------
    // 1. Generate screen texture for compute output
    // -------------------------
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0,
        GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind for safety

    // -------------------------
    // 2. Setup full-screen quad VAO/VBO
    // -------------------------
    float quadVerts[] = {
        // positions   // texcoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    GLuint quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // TexCoord attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0); // unbind VAO for safety


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

    // set up vbo for vert data 
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);

    // Set up ebo for debug
    // Raster index buffer (EBO)
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(uint32_t), allIndices.data(), GL_STATIC_DRAW);

    // Construct vao for debug view
    glGenVertexArrays(1, &debugVao);
    glBindVertexArray(debugVao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(0);

    modelLoc = glGetUniformLocation(debugShader->get(), "uModel");
    viewLoc = glGetUniformLocation(debugShader->get(), "uView");
    projLoc = glGetUniformLocation(debugShader->get(), "uProj");
}

void updateFrame(GLFWwindow* window) {

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        if (!mouseHeld) {
            mouseHeld = true;
            computeView = !computeView;
        }
    }
    else {
        mouseHeld = false;
    }

    double timeDelta = getTimeDeltaSeconds();
    camera->UpdateCameraPos(timeDelta);
}


void compute() {
    compShader->activate();
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform3fv(glGetUniformLocation(compShader->get(), "uCamPos"), 1, &camera->pos[0]);
    glUniformMatrix4fv(glGetUniformLocation(compShader->get(), "uInvPV"), 1, GL_FALSE, &camera->invPV[0][0]);

    glDispatchCompute(
        (GLuint)(windowWidth / 16 + 1),
        (GLuint)(windowHeight / 16 + 1),
        1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    compShader->deactivate();
}

void drawComp() {
    shader->activate();
    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shader->get(), "screenTex"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    shader->deactivate();
}

void drawDebug()
{
    debugShader->activate();
    glBindVertexArray(debugVao);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &camera->model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera->view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &camera->projection[0][0]);

    /*
    // Draw all meshes
    for (const auto& mesh : meshInfos)
    {
        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            mesh.indexCount,
            GL_UNSIGNED_INT,
            (void*)(mesh.indexOffset * sizeof(uint32_t)),
            mesh.vertexOffset
        );
    }*/

    glDrawElements(GL_TRIANGLES, allIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    debugShader->deactivate();
}

void renderFrame(GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    if (computeView) {
       // glDisable(GL_DEPTH_TEST);
        compute();
        drawComp();
        //std::cout << "COMP" << std::endl;
    }
    else {
        //glEnable(GL_DEPTH_TEST);
        drawDebug();
        //std::cout << "DEBUG" << std::endl;
    }
}
