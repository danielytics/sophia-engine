#version 330 core
out vec4 out_color;
in vec2 texCoordinates;

uniform sampler2D debugTexture;
uniform int debugMode;

void main()
{
	if (debugMode == 0) {
		out_color = vec4(texture(debugTexture, texCoordinates).rgb, 0.8);
	} else if (debugMode == 1) {
		float val = texture(debugTexture, texCoordinates).a;
		out_color = vec4(val, val, val, 0.8);
	}
}
