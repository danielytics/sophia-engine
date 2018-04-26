#version 330 core
layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_UV;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out VertexData {
	vec3 position;
	vec2 textureCoordinates;
	flat int image;
} vertex;

uniform samplerBuffer u_tbo_tex;

void main() {
	int offset = gl_InstanceID * 3;
	float x  = texelFetch(u_tbo_tex, offset + 0).r;
	float y  = texelFetch(u_tbo_tex, offset + 1).r;
	vertex.image = int(texelFetch(u_tbo_tex, offset + 2).r);
	vec4 position = view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);
	gl_Position = projection * position;
	vertex.position = vec3(position);
	vertex.textureCoordinates = in_UV;
}
