#version 330 core

layout (location = 0) in vec3 aPosition;

uniform vec4 uPosition;
uniform mat4 uView;
uniform mat4 uProjection;
uniform int uMode;

void main()
{	
	if(uMode == 0)
	{
		gl_Position = uProjection * uView * uPosition;//vec4(aPosition, 1.0);
	}
	else
	{
		gl_Position = uView * uPosition;//vec4(aPosition, 1.0);
	}
    
}