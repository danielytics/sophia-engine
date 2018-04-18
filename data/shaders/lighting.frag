#version 330 core
in VertexData {
	vec3 normal;
	vec3 position;
} fragment;

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	mat4 lightSpaceMatrix;
};

const int NUM_LIGHTS = 2;

uniform Material material;
uniform Light lights[NUM_LIGHTS];
uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	return 1.0 - closestDepth;
}

vec3 lightingCalculation(Light light, float shadow)
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(fragment.normal);
	vec3 lightDir = normalize(light.position - fragment.position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDir = normalize(-fragment.position);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	// simple attenuation
	float max_distance = 5.0;
	float distance = length(light.position - fragment.position);
	float attenuation = 1.0 / pow(distance, 2);

	//	diffuse *= attenuation;
	//	specular *= attenuation;

	// combine ambient, diffuse and specular, with shadowing applied
	return ambient + ((1.0 - shadow) * (diffuse + specular));
}

void main()
{
	vec3 lighting = vec3(0.0);

	// apply lights
	for(int i = 0; i < 1; ++i) {
		float shadow = shadowCalculation(lights[i].lightSpaceMatrix * vec4(fragment.position, 1.0));
		if (i > 0) shadow = 0.0;
		lighting += lightingCalculation(lights[i], shadow);
	}

	// Apply gamma-corrected lighting
	FragColor = vec4(pow(lighting, vec3(1.0/2.2)), 1.0);
//	vec4 fragPosLightSpace = lights[0].lightSpaceMatrix * vec4(fragment.position, 1.0);
//	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//	// transform to [0,1] range
//	projCoords = projCoords * 0.5 + 0.5;
//	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//	float closestDepth = texture(shadowMap, projCoords.xy).r;
//	float currentDepth = projCoords.z;
//	FragColor = vec4(currentDepth - closestDepth, closestDepth - currentDepth, closestDepth, 1.0);
////	float depth = shadowCalculation(lights[0].lightSpaceMatrix * vec4(fragment.position, 1.0));
//	FragColor = vec4(depth, depth, depth, 1.0);
}
