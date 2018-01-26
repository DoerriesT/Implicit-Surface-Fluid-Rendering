#version 330 core

#define MAX_PARTICLES = 20
#define MAX_RADIUS = 7

layout(location = 0) out vec4 oFragColor;

in vec3 vCameraPosition;

uniform vec3 uParticles[20];
uniform int uNumParticles;
uniform vec2 uViewPortSize;
uniform int uMode;

const float ISO_VALUE = 0.5;

float scalarFieldLinear(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < uNumParticles; ++i)
	{
		sum += max(0.0, 4 - distance(position, uParticles[i]));
	}	
	return sum;
}

float scalarFieldExp(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < uNumParticles; ++i)
	{
		sum += exp(-0.5 * distance(position, uParticles[i]));
	}	
	return sum;
}

vec3 calculateNormalLinear(vec3 position, float epsilon)
{
	float nX = scalarFieldLinear(vec3(position.x + epsilon, position.yz)) - scalarFieldLinear(vec3(position.x - epsilon, position.yz));
	float nY = scalarFieldLinear(vec3(position.x, position.y + epsilon, position.z)) - scalarFieldLinear(vec3(position.x, position.y - epsilon, position.z));
	float nZ = scalarFieldLinear(vec3(position.xy, position.z + epsilon)) - scalarFieldLinear(vec3(position.xy, position.z- epsilon));
	return -normalize(vec3(nX, nY, nZ));
}

vec3 calculateNormalExp(vec3 position, float epsilon)
{
	float nX = scalarFieldExp(vec3(position.x + epsilon, position.yz)) - scalarFieldExp(vec3(position.x - epsilon, position.yz));
	float nY = scalarFieldExp(vec3(position.x, position.y + epsilon, position.z)) - scalarFieldExp(vec3(position.x, position.y - epsilon, position.z));
	float nZ = scalarFieldExp(vec3(position.xy, position.z + epsilon)) - scalarFieldExp(vec3(position.xy, position.z- epsilon));
	return -normalize(vec3(nX, nY, nZ));
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
		float epsilon = 0.001f;
		vec3 ray = normalize(vCameraPosition) * 0.0001;
		bool found = false;
		vec3 currentPosition;
		for (int i = 0; i < 10; ++i)
		{
			currentPosition = vCameraPosition - i * ray;
			float value = scalarFieldLinear(currentPosition);
			if(value >= ISO_VALUE)
			{
				found = true;
				float lowerBound = float(i - 1);
				float upperBound = float(i);
				float mid = 0.0;
				for(int j = 0; j < 10; ++j)
				{
					mid = (upperBound - lowerBound) * 0.5;
					currentPosition = vCameraPosition - mid * ray;
					value = scalarFieldLinear(currentPosition);
					float difference = value - ISO_VALUE;
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
				

				vec3 N = calculateNormalLinear(currentPosition, epsilon);
				vec3 L = normalize(vec3(1.0));
				// compute common factors
				vec3 H = normalize(-vCameraPosition+L);
				float NdotL = clamp(dot(N,L), 0.0, 1.0);
				float NdotH = clamp(dot(N,H), 0.0, 1.0);
		
				float specular = pow(NdotH, 32);
				vec3 color =  vec3(0.1) + NdotL * vec3(0.1, 0.1, 1.0) + specular * vec3(1.0);

				oFragColor = vec4(color, 1.0);
			}
		}
		if(!found)
		{
			oFragColor = vec4(texCoord, 0.0, 0.0);
		}
	}
	else
	{
		float epsilon = 0.001f;
		vec3 ray = normalize(vCameraPosition) * 0.01;
		bool found = false;
		vec3 currentPosition;
		for (int i = 0; i < 10; ++i)
		{
			currentPosition = vCameraPosition - i * ray;
			float value = scalarFieldExp(currentPosition);
			if(value >= ISO_VALUE)
			{
				found = true;
				float lowerBound = float(i - 1);
				float upperBound = float(i);
				float mid = 0.0;
				for(int j = 0; j < 10; ++j)
				{
					mid = (upperBound - lowerBound) * 0.5;
					currentPosition = vCameraPosition - mid * ray;
					value = scalarFieldExp(currentPosition);
					float difference = value - ISO_VALUE;
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
				

				vec3 N = calculateNormalExp(currentPosition, epsilon);
				vec3 L = normalize(vec3(1.0));
				// compute common factors
				vec3 H = normalize(-vCameraPosition+L);
				float NdotL = clamp(dot(N,L), 0.0, 1.0);
				float NdotH = clamp(dot(N,H), 0.0, 1.0);
		
				float specular = pow(NdotH, 32);
				vec3 color =  vec3(0.1) + NdotL * vec3(139.0/255.0,69.0/255.0,1.09/255.0) + specular * vec3(1.0);

				oFragColor = vec4(color, 1.0);
			}
		}
		if(!found)
		{
			oFragColor = vec4(texCoord, 0.0, 0.0);
		}
	}
	
}