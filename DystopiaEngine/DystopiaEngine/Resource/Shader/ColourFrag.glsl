#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

uniform vec4 vColor;

layout(location = 0) out vec4 fragColor;


void main()
{
  fragColor = vColor;
}

