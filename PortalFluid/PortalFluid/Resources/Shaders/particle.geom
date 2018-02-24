#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

out vec3 vViewSpacPos;

uniform mat4 uProjection;

const float scale = 5.0;

void main() 
{    
	// point position
	vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 viewSpacePos;
	
	// construct a quad consisting of two triangles around the given point.
	// the quad is scaled by a constant scale value and then transformed into screen space

    viewSpacePos = pos + vec3(1.0, 1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();

    viewSpacePos = pos + vec3(1.0, -1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
	
    viewSpacePos = pos + vec3(-1.0, 1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
    
    EndPrimitive();
	
    viewSpacePos = pos + vec3(1.0, -1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();

    viewSpacePos = pos + vec3(-1.0, -1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
	
    viewSpacePos = pos + vec3(-1.0, 1.0, 1.0) * scale;
	vViewSpacPos = viewSpacePos;
    gl_Position = uProjection * vec4(viewSpacePos, 1.0); 
    EmitVertex();
    
    EndPrimitive();
}