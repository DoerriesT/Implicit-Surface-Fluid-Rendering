#version 330 core

#define MAX_PARTICLES = 20
#define MAX_RADIUS = 7

layout(location = 0) out vec4 oFragColor;

in vec3 vCameraPosition;

uniform vec3 uParticles[20];
uniform int uNumParticles;
uniform vec2 uViewPortSize;
uniform int uMode;
uniform samplerCube uEnvironmentMap;
uniform mat4 uInverseView;

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

vec3 envShading(vec3 N, vec3 V)
{
	vec3 reflection = texture(uEnvironmentMap, reflect(-V, N)).rgb;

	float refractionRed   = texture(uEnvironmentMap, refract(V,-N, 0.7 - 0.03)).r;
	float refractionGreen = texture(uEnvironmentMap, refract(V,-N, 0.7)).g;
	float refractionBlue  = texture(uEnvironmentMap, refract(V,-N, 0.7 + 0.03)).b;

	vec3 refraction = vec3(refractionRed, refractionGreen, refractionBlue);

	float fresnel = pow(1.0 - dot(N, V), 2);
	fresnel = 1.5 * fresnel + 0.1;

	float NdotV = clamp(dot(N, V), 0.0, 1.0);
	vec3 color = mix(refraction, reflection, fresnel);
	return mix(color, texture(uEnvironmentMap, -N).rgb, NdotV);
}

vec3 blinnPhong(vec3 N, vec3 V)
{
	vec3 L = normalize(vec3(1.0));
	vec3 H = normalize(V+L);
	float NdotL = clamp(dot(N,L), 0.0, 1.0);
	float NdotH = clamp(dot(N,H), 0.0, 1.0);
		
	float specular = pow(NdotH, 32);
	return vec3(0.1) + NdotL * vec3(139.0/255.0,69.0/255.0,1.09/255.0) + specular * vec3(1.0);
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
		float epsilon = 0.01f;
		vec3 ray = normalize(vCameraPosition);
		vec3 currentPosition = vCameraPosition;
		vec3 previousPosition = vCameraPosition;
		bool found = false;
		for (int i = 0; i < 10; ++i)
		{
			if(scalarFieldLinear(currentPosition) >= ISO_VALUE)
			{
				found = true;
				vec3 lowerBound = previousPosition;
				vec3 upperBound = currentPosition;
				for(int j = 0; j < 10; ++j)
				{
					currentPosition = (lowerBound + upperBound) * 0.5;
					if (scalarFieldLinear(currentPosition) > ISO_VALUE)
					{
						upperBound = currentPosition;
					}
					else
					{
						lowerBound = currentPosition;
					}
				}
				
				currentPosition = (lowerBound + upperBound) * 0.5;

				vec3 N = calculateNormalLinear(currentPosition, epsilon);
				vec3 V = normalize(-vCameraPosition);

				N = (uInverseView * vec4(N, 0.0)).xyz;
				V = (uInverseView * vec4(V, 0.0)).xyz;

				oFragColor = vec4(envShading(N, V), 1.0);
			}
			previousPosition = currentPosition;
			currentPosition += ray;
		}
		if(!found)
		{
			oFragColor = vec4(0.0);
		}
	}
	else
	{
		float epsilon = 0.01f;
		vec3 ray = normalize(vCameraPosition);
		vec3 currentPosition = vCameraPosition;
		vec3 previousPosition = vCameraPosition;
		bool found = false;
		for (int i = 0; i < 10; ++i)
		{
			if(scalarFieldExp(currentPosition) >= ISO_VALUE)
			{
				found = true;
				vec3 lowerBound = previousPosition;
				vec3 upperBound = currentPosition;
				for(int j = 0; j < 10; ++j)
				{
					currentPosition = (lowerBound + upperBound) * 0.5;
					if (scalarFieldExp(currentPosition) > ISO_VALUE)
					{
						upperBound = currentPosition;
					}
					else
					{
						lowerBound = currentPosition;
					}
				}
				
				currentPosition = (lowerBound + upperBound) * 0.5;

				vec3 N = calculateNormalExp(currentPosition, epsilon);
				vec3 V = normalize(-vCameraPosition);

				N = (uInverseView * vec4(N, 0.0)).xyz;
				V = (uInverseView * vec4(V, 0.0)).xyz;

				oFragColor = vec4(envShading(N, V), 1.0);
			}
			previousPosition = currentPosition;
			currentPosition += ray;
		}
		if(!found)
		{
			oFragColor = vec4(0.0);
		}
	}
	
}