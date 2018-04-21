#version 330 core
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoords;
} fragment;

layout (location = 0) out vec3 gBufferPosition;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec4 gBufferAlbedo;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	// store the fragment position vector in the first gbuffer texture
	gBufferPosition = fragment.position;
	// also store the per-fragment normals into the gbuffer
	gNormal = normalize(fragment.normal);
	// and the diffuse per-fragment color
	gBufferAlbedo.rgb = texture(texture_diffuse1, fragment.textureCoords).rgb;
	// store specular intensity in gBufferAlbedo's alpha component
	gBufferAlbedo.a = texture(texture_specular1, fragment.textureCoords).r;
}
