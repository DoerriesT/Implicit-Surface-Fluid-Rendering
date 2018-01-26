#version 330 core

#define MAX_PARTICLES = 20
#define MAX_RADIUS = 7
#define ISO_VALUE = 0.5

layout(location = 0) out vec4 oFragColor;

in vec3 vCameraPosition;


uniform vec3 uParticles[7];
uniform vec2 uViewPortSize;
uniform int uMode;

float scalarFieldSimple(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < 7; ++i)
	{
		sum += max(0.0, 7 - distance(position, uParticles[i]));
	}	
	return sum;
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy / uViewPortSize;

	if (uMode == 0)
	{
		oFragColor = vec4(1.0, 1.0, 0.0, 1.0);
	}
	else if (uMode == 1)
	{
		oFragColor = vec4(texCoord, 0.0, 1.0);
	}
	else if (uMode == 2)
	{
		oFragColor = vec4(texCoord, 0.0, 1.0);
	}
	else
	{
		oFragColor = vec4(texCoord, 0.0, 1.0);
	}
	
}