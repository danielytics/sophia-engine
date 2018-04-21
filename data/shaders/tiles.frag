#version 330 core
in vec4 color;
out vec4 fragColor;
in vec2 uv;
in float image;
uniform sampler2DArray u_texture;
void main(void) {
	fragColor = texture(u_texture, vec3(uv, image)).rgba * color;
}
