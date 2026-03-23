#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vNormal;

void main()
{
    // Transform position
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);

    // Transform normal correctly (important if model matrix has scaling)
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * aNormal);
}