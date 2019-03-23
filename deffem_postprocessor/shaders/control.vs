#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    gl_Position = projection * vec4(aPos.xy, 1.0, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}