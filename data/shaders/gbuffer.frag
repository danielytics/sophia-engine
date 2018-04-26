#version 330 core
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoords;
	// MODE_SPRITES
	flat int image;
//	vec4 color;
} fragment;

layout (location = 0) out vec3 gBufferPosition;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec4 gBufferAlbedo;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// renderMode values.
const int MODE_BACKGROUNDS = 0;
const int MODE_BAKED_LIGHTING = 1;
const int MODE_3D_GEOMETRY = 2;
const int MODE_TILES = 3;
const int MODE_SPRITES = 4;
uniform int renderMode; // Same for all fragments in draw call, so conditionals should be fast

uniform sampler2DArray u_texture;

void main()
{
	// store the fragment position vector in the first gbuffer texture
	gBufferPosition = fragment.position;
	// also store the per-fragment normals into the gbuffer
	gBufferNormal = normalize(fragment.normal);

	switch (renderMode) {
	case MODE_SPRITES:
		// and the diffuse per-fragment color
		gBufferAlbedo.rgb = texture(u_texture, vec3(fragment.textureCoords, fragment.image)).rgb;
		// store specular intensity in gBufferAlbedo's alpha component
		gBufferAlbedo.a = 0;
		break;
	default:
		// and the diffuse per-fragment color
		gBufferAlbedo.rgb = texture(texture_diffuse1, fragment.textureCoords).rgb;
		// store specular intensity in gBufferAlbedo's alpha component
		gBufferAlbedo.a = texture(texture_specular1, fragment.textureCoords).r;
		break;
	}
}
