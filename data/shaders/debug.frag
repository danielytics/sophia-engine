#version 330 core
out vec4 out_color;
in vec2 texCoordinates;

uniform sampler2D debugTexture;

void main()
{
	out_color = texture(debugTexture, texCoordinates);
}
