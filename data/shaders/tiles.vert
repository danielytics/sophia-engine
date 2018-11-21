#version 330 core
layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec4 in_Color;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in float in_Image;
uniform mat4 u_projection;
uniform mat4 u_view;
out vec2 uv;
out vec4 color;
out float image;
void main() {
	gl_Position =  u_projection * u_view * vec4(in_Position, 0.0, 1.0);
	color = in_Color;
	uv = in_UV;
	image = in_Image;
}
