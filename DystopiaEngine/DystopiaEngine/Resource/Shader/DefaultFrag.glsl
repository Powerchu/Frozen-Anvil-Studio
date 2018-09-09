#version 330 core

// in vec3 vPosition;
in vec2 vUV;
// in vec3 vColor;

out vec4 fragColor;

uniform sampler2D texSampler;


void main()
{
  // Ignore colour blending for now
  fragColor = texture(texSampler, vUV);
}