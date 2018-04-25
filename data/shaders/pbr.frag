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
	vec3 fragPos = texture(gPosition, texCoordinates).rgb;
	vec3 normal = texture(gNormal, texCoordinates).rgb;
	vec3 albedo = texture(gAlbedoSpec, texCoordinates).rgb;
	float specular = texture(gAlbedoSpec, texCoordinates).a;

	// then calculate lighting as usual
	vec3 lighting = albedo * 0.1; // hard-coded ambient component
	vec3 viewDir = normalize(viewPos - fragPos);
	for(int i = 0; i < NR_LIGHTS; ++i)
	{
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - fragPos);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].Color;
		lighting += diffuse;
	}

	FragColor = vec4(lighting, 1.0);
}
