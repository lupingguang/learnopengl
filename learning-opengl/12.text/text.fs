#version 130

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
   // if (sampled.w == 0) {
   //     discard;
   // }
    color = vec4(textColor, 1.0) * sampled;
}
