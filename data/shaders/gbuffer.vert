#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;


out VertexData {
	vec3 normal;
	vec3 position;
	vec2 textureCoords;
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
	vec3 vm_position = view * model * vec4(in_position, 1.0);
	gl_Position = projection * vm_position;
	vertex.position = vec3(vm_position);
	vertex.normal = normal * in_normal;
	vertex.textureCoords = in_uv;
}
