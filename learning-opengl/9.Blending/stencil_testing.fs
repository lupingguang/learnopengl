#version 130

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{             
    FragColor = texture(texture1, TexCoords);
}
