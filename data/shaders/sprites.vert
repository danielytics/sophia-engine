#version 330 core
layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_UV;
uniform samplerBuffer u_tbo_tex;
uniform mat4 u_projection;
uniform mat4 u_view;
out vec2 uv;
out float image;
out vec4 color;
void main() {
	int offset = gl_InstanceID * 3;
	float x  = texelFetch(u_tbo_tex, offset + 0).r;
	float y  = texelFetch(u_tbo_tex, offset + 1).r;
	image = texelFetch(u_tbo_tex, offset + 2).r;
	gl_Position = u_projection * u_view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);
	uv = in_UV;
}
