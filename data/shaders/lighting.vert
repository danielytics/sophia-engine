#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;


out VertexData {
	vec3 normal;
	vec3 position;
} vertex;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat3 normal;

void main()
{
	gl_Position = projection * view * model * vec4(in_position, 1.0);
	vertex.position = vec3(model * view * vec4(in_position, 1.0));
	vertex.normal = normal * in_normal;
}
