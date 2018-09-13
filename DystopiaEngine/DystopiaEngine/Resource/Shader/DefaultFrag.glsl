#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

layout(location = 0) out vec4 fragColor;

uniform float Gamma;
uniform sampler2D texSampler;


void main()
{
  // Ignore colour blending for now
  vec4 color = texture(texSampler, vUV);
  
  // Gamma correction
  color.rgb = pow(color.rgb, vec3(1.f / Gamma));
  
  
  fragColor = color;
}

