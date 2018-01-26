#version 330

out vec3 vRay;

uniform mat4 uInverseModelViewProjection;

void main()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
	float y = -1.0 + float((gl_VertexID & 2) << 1);
    gl_Position = vec4(x, y, 1.0, 1.0);
	vec4 ray = uInverseModelViewProjection * vec4(gl_Position.xy, -1.0,  1.0);
	vRay = (ray/ray.w).xyz;
}
