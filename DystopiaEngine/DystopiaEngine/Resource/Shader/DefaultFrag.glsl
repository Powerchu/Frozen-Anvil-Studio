#version 330 core

in vec3 vPosition;
in vec2 vUV;
// in vec3 vColor;

layout(location = 0) out vec4 fragColor;

uniform sampler2D texSampler;


void main()
{
  // Ignore colour blending for now
  fragColor = vec4(1, 0, 1, 0); //texture(texSampler, vUV);
}

