#version 130

in vec3 aPos; // 位置变量的属性位置值为0
in vec2 aTexCoords;
in vec3 aOffset;

out vec2 TexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model *  vec4(aPos + aOffset, 1.0);
}
