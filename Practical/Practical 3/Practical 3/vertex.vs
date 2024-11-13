#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
uniform mat4 trans;
void main()
{
	gl_Position = project * view * model * vec4(vPos,1.0);
	outColor=vColor;
}