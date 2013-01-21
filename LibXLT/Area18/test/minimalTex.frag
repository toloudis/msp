#version 410

uniform sampler2D gTexture;

smooth in vec4 CameraPos;
smooth in vec2 UV;
out vec4 out_Color;

void main(void)
{
	out_Color = texture(gTexture, UV, 0);
}