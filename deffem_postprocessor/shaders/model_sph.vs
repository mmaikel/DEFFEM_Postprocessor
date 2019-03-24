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
    float posX = aPos.x > intersection.x ? 1000 : aPos.x;
    float posY = aPos.y > intersection.y ? 1000 : aPos.y;
    float posZ = aPos.z > intersection.z ? 1000 : aPos.z;

    gl_Position = projection * view * model * vec4(posX, posY, posZ, 1.0);

    ourColor = vec4(aColor, 1.0);
}