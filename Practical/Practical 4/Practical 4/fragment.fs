#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 Fragcolor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
	//lightPos = vec3(model * mat4(lightPos,1.0f));

	float ambientLight = 0.8f;
	vec3 ambient = ambientLight * lightColor;

	vec3 normalVector = normalize (Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diffuseLight = max (dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;

	int shininess = 128;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect (-lightDir, normalVector);
	float specularLight = max (dot (viewDir, reflectDir), 0.0)                                                                           ;
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;

	Fragcolor = vec4(result, 1.0f);
}