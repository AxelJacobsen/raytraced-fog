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
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>



namespace Fog
{
    class Camera
    {
    public:
        glm::vec3 pos;
        glm::vec3 direction;
        glm::vec3 target;
        glm::vec3 up;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 PV;
        glm::mat4 invPV;


        float focal_length = 1.0;
        
        Camera::Camera() : 
            pos(glm::vec3{ 0.0, 5.0, 5.0 }), 
            direction(glm::vec3{ 0.0, 0.0, -1.0 }),
            target(glm::vec3{ 0.0, 0.0, 0.0 }),
            up(glm::vec3{ 0.0, 1.0, 0.0 }),
           
            model(glm::mat4(1.0f)),
            view(glm::lookAt(pos, target, up)),
            projection(glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f)),
            PV(projection * view),
            invPV(glm::inverse(PV))
        {};

        void Camera::SetPV(glm::mat4 P, glm::mat4 V) {
            view = V;
            projection = P;
            PV = projection * view;
            invPV = glm::inverse(PV);
        };

        void Camera::UpdateCamera() {
            view = glm::lookAt(pos, target, up);
            projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);
            PV = projection * view;
            invPV = glm::inverse(PV);
        }

        void Camera::UpdateCameraPos(GLfloat deltaTime)
        {
            // Extract movement information from the view matrix
            glm::vec3 dirX(view[0][0], view[1][0], view[2][0]);
            glm::vec3 dirY(view[0][1], view[1][1], view[2][1]);
            glm::vec3 dirZ(view[0][2], view[1][2], view[2][2]);

            // Alter position in the appropriate direction
            glm::vec3 fMovement(0.0f, 0.0f, 0.0f);

            if (keysInUse[GLFW_KEY_W])  // forward
                fMovement -= dirZ;

            if (keysInUse[GLFW_KEY_S])  // backward
                fMovement += dirZ;

            if (keysInUse[GLFW_KEY_A])  // left
                fMovement -= dirX;

            if (keysInUse[GLFW_KEY_D])  // right
                fMovement += dirX;

            if (keysInUse[GLFW_KEY_E])  // vertical up
                fMovement += dirY;

            if (keysInUse[GLFW_KEY_Q])  // vertical down
                fMovement -= dirY;

            // Trick to balance PC speed with movement
            GLfloat velocity = cMovementSpeed * deltaTime;

            // Update camera position using the appropriate velocity
            pos += fMovement * velocity;

            // Update the view matrix based on the new information
            updateViewMatrix();
        }

        void Camera::handleCursorPosInput(double xpos, double ypos)
        {
            fYaw = (xpos - lastXPos) * cMouseSensitivity;
            fPitch = (ypos - lastYPos) * cMouseSensitivity;

            lastXPos = xpos;
            lastYPos = ypos;
        }

        void handleKeyboardInputs(int key, int action)
        {
            // Keep track of pressed/released buttons
            if (key >= 0 && key < 512)
            {
                if (action == GLFW_PRESS)
                {
                    keysInUse[key] = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    keysInUse[key] = false;
                }
            }
        }

    private:
        glm::quat cQuaternion;
        GLfloat fPitch = 0.0f;
        GLfloat fYaw = 0.0f;
        GLfloat lastXPos = 0.0f;
        GLfloat lastYPos = 0.0f;
        GLfloat cMovementSpeed = 0.5;
        GLfloat cMouseSensitivity;
        GLboolean keysInUse[512];

        Camera(Camera const&) = delete;
        Camera& operator = (Camera const&) = delete;

        void Camera::updateViewMatrix()
        {
            // Create quaternions given the current pitch and yaw
            glm::quat qPitch = glm::quat(glm::vec3(fPitch, 0.0f, 0.0f));
            glm::quat qYaw = glm::quat(glm::vec3(0.0f, fYaw, 0.0f));

            // Reset pitch and yaw values for the current rotation
            fPitch = 0.0f;
            fYaw = 0.0f;

            // Update camera quaternion and normalise
            cQuaternion = qPitch * qYaw * cQuaternion;
            cQuaternion = glm::normalize(cQuaternion);

            // Build rotation matrix using the camera quaternion
            glm::mat4 matRotation = glm::mat4_cast(cQuaternion);

            // Build translation matrix
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f), -pos);

            // Update view matrix
            view = matRotation * matTranslate;

            UpdateCamera();
        }
    };
}

#endif
