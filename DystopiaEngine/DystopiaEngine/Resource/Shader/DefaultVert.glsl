#version 330 core

uniform mat4 ProjectViewMat;
uniform mat4 ModelMat;

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNorm;
layout(location = 2) in vec2 vertexUV;
// layout(location = 3) in vec3 vertexColor;

out vec3 vPosition;
out vec2 vUV;
out vec3 vColor;

void main()
{
  vec4 temp = ModelMat * vec4(vertexPos, 1.f);
	vPosition = temp.xyz;
	vUV       = vertexUV;
  //vColor    = vertexColor;

	gl_Position = ProjectViewMat * temp;
}

