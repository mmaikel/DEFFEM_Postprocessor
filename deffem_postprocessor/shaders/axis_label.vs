#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 model_t;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model_t * model * vec4(aPos, 1.0);

    float tx;
    float ty;

    if(aTexCoord.x == 0.0 && aTexCoord.y == 1.0) {
        tx = 1.0;
    }
    else if (aTexCoord.x == 1.0 && aTexCoord.y == 1.0) {
        tx = 0.0;
    }
    else if (aTexCoord.x == 1.0 && aTexCoord.y == 0.0) {
        tx = 0.0;
    }
    else if (aTexCoord.x == 0.0 && aTexCoord.y == 0.0) {
        tx = 1.0;
    }

    TexCoord = vec2(tx, aTexCoord.y);
}