#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

layout(location = 0) out vec4 fragColor;


void main()
{
  fragColor = vec4(0.7, 0.7, 0.7, 1);
}

