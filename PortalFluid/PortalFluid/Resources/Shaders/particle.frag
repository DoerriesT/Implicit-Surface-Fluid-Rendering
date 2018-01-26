#version 330 core

#define MAX_PARTICLES = 20
#define MAX_RADIUS = 7
#define ISO_VALUE = 2.5

layout(location = 0) out vec4 oFragColor;

in vec3 vCameraPosition;

uniform vec3 uParticles[20];
uniform int uNumParticles;
uniform vec2 uViewPortSize;
uniform int uMode;

float scalarFieldSimple(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < uNumParticles; ++i)
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
		vec3 ray = normalize(vCameraPosition);
		bool found = false;
		for (int i = 0; i < 10; ++i)
		{
			vec3 currentPosition = vCameraPosition - i * ray;
			float value = scalarFieldSimple(currentPosition);
			if(value >= 0.5)
			{
				found = true;
				float lowerBound = float(i - 1);
				float upperBound = float(i);
				for(int j = 0; j < 10; ++j)
				{
					float mid = (upperBound - lowerBound) * 0.5;
					value = scalarFieldSimple(vCameraPosition - mid * ray);
					float difference = value - 0.5;
					if (difference > 0.0)
					{
						upperBound = mid;
					}
					else if (difference < 0.0)
					{
						lowerBound = mid;
					}
					else
					{
						break;
					}
				}
				oFragColor = vec4(value.xxx, 1.0);
			}
		}
		if(!found)
		{
			discard;
		}
	}
	else
	{
		oFragColor = vec4(vCameraPosition, 1.0);
	}
	
}