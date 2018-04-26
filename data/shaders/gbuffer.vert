#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;


out VertexData {
	vec3 normal;
	vec3 position;
	vec2 textureCoords;
	// MODE_SPRITES
	flat int image;
//	vec4 color;
} vertex;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat3 normal;

// renderMode values.
const int MODE_BACKGROUNDS = 0;
const int MODE_BAKED_LIGHTING = 1;
const int MODE_3D_GEOMETRY = 2;
const int MODE_TILES = 3;
const int MODE_SPRITES = 4;
uniform int renderMode; // Same for all fragments in draw call, so conditionals should be fast

// MODE_SPRITES
uniform samplerBuffer u_tbo_tex;

void main()
{
	vec4 vm_position;

	switch (renderMode) {
	case MODE_SPRITES:
		int offset = gl_InstanceID * 3;
		float x  = texelFetch(u_tbo_tex, offset + 0).r;
		float y  = texelFetch(u_tbo_tex, offset + 1).r;
		vertex.image = int(texelFetch(u_tbo_tex, offset + 2).r);
		vm_position = view * vec4(in_position.x + x, in_position.y + y, 0.0, 1.0);
		break;
	default:
		vm_position = view * model * vec4(in_position, 1.0);
		vertex.image = 0;
		break;
	}
	vertex.normal = normal * in_normal;
	vertex.textureCoords = in_uv;
	vertex.position = vec3(vm_position);;
	gl_Position = projection * vm_position;
}
