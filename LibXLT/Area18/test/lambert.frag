#version 410

uniform vec4 color;

void shade(vec3 P, vec3 n, vec2 uv, vec4 lcolor, vec3 ldir, out vec4 Co)
{
	float d = max(0, dot(-ldir, n));
	Co = vec4((lcolor.xyz * color.xyz) * d, color.w);
}