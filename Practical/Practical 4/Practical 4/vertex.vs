#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNormal;

out vec3 Normal;
out vec3 FragPos;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
	gl_Position = project * view * model * vec4(vPos,1.0);
	
	FragPos = vec3(model * vec4(vPos, 1.0));
	
	Normal = mat3(transpose(inverse(model))) * vNormal; // 법선 변환
}