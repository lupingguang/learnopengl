#version 130
in vec3 aPos; // 位置变量的属性位置值为0
in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;  
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
