#version 330

layout(location = 0) out vec4 oColor;

in vec3 vRay;

uniform samplerCube uEnvironmentMap;

void main()
{
    vec3 sample = texture(uEnvironmentMap, normalize(vRay)).rgb;
	oColor = vec4(sample, 1.0);
}