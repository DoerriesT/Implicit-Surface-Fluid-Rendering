#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;
uniform int uMode;

void main()
{	
	if(uMode == 0)
	{
		// point mode; transform particle positions directly to screen space
		gl_Position = uProjection * uView * vec4(aPosition, 1.0);
	}
	else
	{
		// quad mode; transform particle positions to view space
		gl_Position = uView * vec4(aPosition, 1.0);
	}
    
}