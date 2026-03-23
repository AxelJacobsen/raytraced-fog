#version 430 core

in vec3 vNormal;
out vec4 FragColor;

void main()
{
    // Map [-1,1] → [0,1] for visualization
    vec3 color = vNormal * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}