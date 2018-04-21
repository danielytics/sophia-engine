#version 330 core
in vec2 uv;
in float image;
out vec4 fragColor;
uniform sampler2DArray u_texture;
void main(void) {
	fragColor = texture(u_texture, vec3(uv, image)).rgba;
}
