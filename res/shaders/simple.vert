#version 430 core

out vec2 uv;

void main()
{
    vec2 positions[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2( 1.0,  1.0),

        vec2(-1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  1.0)
    );

    vec2 pos = positions[gl_VertexID];

    uv = (pos + 1.0) * 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}