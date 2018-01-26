#version 330 core

layout(location = 0) out vec4 oFragColor;

uniform vec2 uViewPortSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy / uViewPortSize;
	oFragColor = vec4(texCoord, 0.0, 1.0);
}