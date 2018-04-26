#version 330 core
out vec4 FragColor;

in vec2 texCoordinates;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 Position;
	vec3 Color;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
	// retrieve data from G-buffer
	vec4 gbuf_position = texture(gPosition, texCoordinates);
	vec4 gbuf_normal = texture(gNormal, texCoordinates);
	vec4 gbuf_albedo = texture(gAlbedoSpec, texCoordinates);
	vec3 fragPos = gbuf_position.rgb;
	vec3 normal = gbuf_normal.rgb;
	vec3 albedo = gbuf_albedo.rgb;
	float ao = gbuf_position.a;
	float roughness = gbuf_normal.a;
	float specular = gbuf_albedo.a;

	// then calculate lighting as usual
	vec3 lighting = albedo * 0.1; // hard-coded ambient component
	vec3 viewDir = normalize(viewPos - fragPos);
	for(int i = 0; i < 0 /* NR_LIGHTS */; ++i)
	{
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - fragPos);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].Color;
		lighting += diffuse;
	}

//	FragColor = vec4(lighting, 1.0);
	FragColor = vec4(albedo, 1.0);
}
