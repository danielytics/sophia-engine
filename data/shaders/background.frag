#version 330 core
in VertexData {
	vec3 position;
	vec2 texCoordinates;
} fragment;
layout (location = 0) out vec4 gBufferPosition;
layout (location = 1) out vec4 gBufferNormal;
layout (location = 2) out vec4 gBufferAlbedo;

uniform sampler2DArray u_texture;

void main()
{
	vec3 normal = vec3(0.0, 1.0, 0.0);
	vec3 albedo = texture(u_texture, vec3(fragment.texCoordinates, 0)).rgb;
	float ao = fragment.texCoordinates.x;
	float roughness = fragment.texCoordinates.y;
	float specular = 0.0;

	gBufferPosition = vec4(fragment.position, ao);
	gBufferNormal = vec4(normal, roughness);
	gBufferAlbedo = vec4(albedo, specular);
}
