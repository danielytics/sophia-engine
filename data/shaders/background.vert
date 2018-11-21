#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoordinates;

out VertexData {
	vec3 position;
	vec2 texCoordinates;
} vertex;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
	vec4 position = view * vec4(in_position, 1.0);
	vertex.position = position.xyz;
	vertex.texCoordinates = in_texCoordinates;
	gl_Position = projection  * position;
}
