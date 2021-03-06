#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec4 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 intersection;

void main()
{
    float posX = aPos.x > intersection.x ? intersection.x : aPos.x;
    float posY = aPos.y > intersection.y ? intersection.y : aPos.y;
    float posZ = aPos.z > intersection.z ? intersection.z : aPos.z;

    gl_Position = projection * view * model * vec4(posX, posY, posZ, 1.0);

    ourColor = vec4(aColor, 1.0);
}