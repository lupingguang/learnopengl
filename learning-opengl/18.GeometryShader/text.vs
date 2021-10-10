#version 130

in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main ()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); // z坐标=0，在原点，w坐标为1，无缩放
    TexCoords = vertex.zw;
}
