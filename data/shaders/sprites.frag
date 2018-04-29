#version 330 core
in VertexData {
	vec3 position;
	vec2 textureCoordinates;
	flat int image;
} fragment;

layout (location = 0) out vec4 gBufferPosition;
layout (location = 1) out vec4 gBufferNormal;
layout (location = 2) out vec4 gBufferAlbedo;

uniform sampler2DArray u_texture;

void main(void) {
	vec4 tex = texture(u_texture, vec3(fragment.textureCoordinates, fragment.image));
	if (tex.a < 1.0) {
		discard; // No early-z for sprites :'(
	}
	vec3 normal = vec3(0.0, 0.0, 1.0);
	vec3 albedo = tex.rgb;
	float ao = fragment.textureCoordinates.x;
	float roughness = fragment.textureCoordinates.y;
	float specular = 0.0;

	gBufferPosition = vec4(fragment.position, ao);
	gBufferNormal = vec4(normal, roughness);
	gBufferAlbedo = vec4(albedo, specular);
}
