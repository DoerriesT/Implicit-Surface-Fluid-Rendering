#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

out vec3 vCameraPosition;

uniform mat4 uProjection;

void main() 
{    
	vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 viewSpacePos;
	float scale = 5.0;

    viewSpacePos = pos + vec3(1.0, 1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();

    viewSpacePos = pos + vec3(1.0, -1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
	
    viewSpacePos = pos + vec3(-1.0, 1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
    
    EndPrimitive();
	
    viewSpacePos = pos + vec3(1.0, -1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();

    viewSpacePos = pos + vec3(-1.0, -1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
	
    viewSpacePos = pos + vec3(-1.0, 1.0, 0.15) * scale;
	vCameraPosition = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
    
    EndPrimitive();
}