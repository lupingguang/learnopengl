#version 130
in vec3 aPos; // 位置变量的属性位置值为0
in vec3 aColor;
in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoords = vec2(aTexCoord.x, aTexCoord.y);
}
