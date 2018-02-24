#version 330 core

#define MAX_PARTICLES = 20
#define MAX_RADIUS = 7

layout(location = 0) out vec4 oFragColor;

in vec3 vViewSpacPos;

// array containing all particle positions
uniform vec3 uParticles[20];
// number of currently simulated particles
uniform int uNumParticles;
// viewport/window size
uniform vec2 uViewPortSize;
// rendering mode (points/uv/linear/exponential)
uniform int uMode;
// environment cube map
uniform samplerCube uEnvironmentMap;
// inverse view matrix
uniform mat4 uInverseView;
// material/substance mode (water/glass/air bubbles/soap bubbles)
uniform int uSubstanceMode;

// desired iso surface value
const float ISO_VALUE = 0.5;

// evaluate the scalar field with a linear term
float scalarFieldLinear(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < uNumParticles; ++i)
	{
		sum += max(0.0, 4 - distance(position, uParticles[i]));
	}	
	return sum;
}

// evaluate the scalar field with an exponential term
float scalarFieldExp(vec3 position)
{
	float sum = 0.0;
	for (int i = 0; i < uNumParticles; ++i)
	{
		sum += exp(-0.5 * distance(position, uParticles[i]));
	}	
	return sum;
}

// calculate surface normal using the linear scalar field evaluation function
vec3 calculateNormalLinear(vec3 position, float epsilon)
{
	float nX = scalarFieldLinear(vec3(position.x + epsilon, position.yz))            - scalarFieldLinear(vec3(position.x - epsilon, position.yz));
	float nY = scalarFieldLinear(vec3(position.x, position.y + epsilon, position.z)) - scalarFieldLinear(vec3(position.x, position.y - epsilon, position.z));
	float nZ = scalarFieldLinear(vec3(position.xy, position.z + epsilon))            - scalarFieldLinear(vec3(position.xy, position.z- epsilon));
	return -normalize(vec3(nX, nY, nZ));
}

// calculate surface normal using the exponential scalar field evaluation function
vec3 calculateNormalExp(vec3 position, float epsilon)
{
	float nX = scalarFieldExp(vec3(position.x + epsilon, position.yz)) - scalarFieldExp(vec3(position.x - epsilon, position.yz));
	float nY = scalarFieldExp(vec3(position.x, position.y + epsilon, position.z)) - scalarFieldExp(vec3(position.x, position.y - epsilon, position.z));
	float nZ = scalarFieldExp(vec3(position.xy, position.z + epsilon)) - scalarFieldExp(vec3(position.xy, position.z- epsilon));
	return -normalize(vec3(nX, nY, nZ));
}

// environment map shading
vec3 envShading(vec3 N, vec3 V)
{
	// sample reflection color value from environment map
	vec3 reflection = texture(uEnvironmentMap, reflect(-V, N)).rgb;

	// initialize refraction value with zero (bad things can happen if you do not initialize every variable)
	vec3 refraction = vec3(0.0);

	// depending on the desired material/substance calculate a refraction ratio and sample the environment map
	switch (uSubstanceMode)
	{
		case 0:
		{
			// water
			float etaAirWater = 1.0003 / 1.3333;
			refraction = texture(uEnvironmentMap, refract(-V,N, etaAirWater)).rgb;
			break;
		}
		case 1:
		{
			// glass
			float etaAirGlass = 1.0003 / 1.5;
			refraction = vec3(texture(uEnvironmentMap, refract(-V,N, etaAirGlass - 0.03)).r, 
							texture(uEnvironmentMap, refract(-V,N, etaAirGlass)).g, 
							texture(uEnvironmentMap, refract(-V,N, etaAirGlass + 0.03)).b);
			break;
		}
		case 2:
		{
			// air
			float etaAirAir = 1.0;
			refraction = texture(uEnvironmentMap, refract(-V,N, etaAirAir)).rgb;
			break;
		}
		case 3:
		{
			// air diffraction
			float etaAirAir = 1.0;
			refraction = vec3(texture(uEnvironmentMap, refract(-V,N, etaAirAir - 0.03)).r, 
							texture(uEnvironmentMap, refract(-V,N, etaAirAir)).g, 
							texture(uEnvironmentMap, refract(-V,N, etaAirAir + 0.03)).b);
			break;
		}
	}

	// calculate fresnel factor
	float fresnel = pow(1.0 - dot(N, V), 2);
	fresnel = 1.5 * fresnel + 0.1;
	
	// lerp between reflection and refraction color based on fresnel factor
	vec3 color = mix(refraction, reflection, fresnel);

	// apply some blinn-phong specular shading
	vec3 L = normalize(vec3(1.0, 1.0, 0.0));
	vec3 H = normalize(V+L);
	float NdotH = clamp(dot(N,H), 0.0, 1.0);
		
	float specular = pow(NdotH, 128);
	return color + specular * vec3(0.7);
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy / uViewPortSize;

	// we are only drawing red points
	if (uMode == 0)
	{
		oFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	// we are drawing quads and color them with the respective uv-coordinate
	else if (uMode == 1)
	{
		oFragColor = vec4(texCoord, 0.0, 1.0);
	}
	// linear scalar field evaluation
	else if (uMode == 2)
	{
		float epsilon = 0.01f;
		// ray direction to current pixel in view space
		vec3 ray = normalize(vViewSpacPos);
		// current position on ray
		vec3 currentPosition = vViewSpacPos;
		// previous position on ray (used for interval bisection)
		vec3 previousPosition = vViewSpacPos;
		// bool indicating wether we hit the desired iso surface value
		bool found = false;

		// march along the ray, evaluate the scalar field each position and test if we passed the desired iso surface value
		for (int i = 0; i < 10; ++i)
		{
			// the closer we get to a particle, the higher the evaluated scalar field value
			// if we passed the desired iso value, perform interval bisection to more precisely
			// calculate the surface point
			if(scalarFieldLinear(currentPosition) >= ISO_VALUE)
			{
				found = true;

				// interval bisection is essentially binary search
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

				// calculate normal and view vector
				vec3 N = calculateNormalLinear(currentPosition, epsilon);
				vec3 V = normalize(-vViewSpacPos);

				// transform them into world space
				N = (uInverseView * vec4(N, 0.0)).xyz;
				V = (uInverseView * vec4(V, 0.0)).xyz;

				// light the pixel with environment mapping
				oFragColor = vec4(envShading(N, V), 1.0);
				break;
			}
			// we have not yet passed the desired iso value, so continue marching along the ray
			else
			{
				previousPosition = currentPosition;
				currentPosition += ray;
			}
		}
		// if we have not hit the desird iso value, make the pixel transparent
		if(!found)
		{
			oFragColor = vec4(0.0);
		}
	}
	// exponential scalar field evaluation
	else
	{
		float epsilon = 0.01f;
		// ray direction to current pixel in view space
		vec3 ray = normalize(vViewSpacPos);
		// current position on ray
		vec3 currentPosition = vViewSpacPos;
		// previous position on ray (used for interval bisection)
		vec3 previousPosition = vViewSpacPos;
		// bool indicating wether we hit the desired iso surface value
		bool found = false;

		// march along the ray, evaluate the scalar field each position and test if we passed the desired iso surface value
		for (int i = 0; i < 10; ++i)
		{
			// the closer we get to a particle, the higher the evaluated scalar field value
			// if we passed the desired iso value, perform interval bisection to more precisely
			// calculate the surface point
			if(scalarFieldExp(currentPosition) >= ISO_VALUE)
			{
				found = true;

				// interval bisection is essentially binary search
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

				// calculate normal and view vector
				vec3 N = calculateNormalExp(currentPosition, epsilon);
				vec3 V = normalize(-vViewSpacPos);

				// transform them into world space
				N = (uInverseView * vec4(N, 0.0)).xyz;
				V = (uInverseView * vec4(V, 0.0)).xyz;

				// light the pixel with environment mapping
				oFragColor = vec4(envShading(N, V), 1.0);
				break;
			}
			// we have not yet passed the desired iso value, so continue marching along the ray
			else
			{
				previousPosition = currentPosition;
				currentPosition += ray;
			}
		}
		// if we have not hit the desird iso value, make the pixel transparent
		if(!found)
		{
			oFragColor = vec4(0.0);
		}
	}
	
}