#version 330 core

in vec3 vPosition;
in vec2 vUV;
// in vec3 vColor;

layout(location = 0) out vec4 fragColor;

uniform float Gamma;
uniform sampler2D texSampler;


void main()
{
  // Ignore colour blending for now
  
  vec4 tempColor = texture(texSampler, vUV);
  vec3 gammaColor = pow(tempColor.rgb, vec3(1.f / Gamma));
  fragColor = vec4(gammaColor.rgb, tempColor.a);
}

