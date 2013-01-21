#version 410

smooth in vec4 CameraPos;
smooth in vec2 UV;
smooth in vec3 CameraNormal;

out vec4 out_Color;

// fwd decl
void light(vec3 P, vec3 n, vec2 uv, out vec4 color, out vec3 incidentDir);
void shade(vec3 P, vec3 n, vec2 uv, vec4 lcolor, vec3 ldir, out vec4 Co);

void main(void)
{
	vec4 ltcolor;
	vec3 ltdir;
	light(CameraPos.xyz, CameraNormal, UV, ltcolor, ltdir);
	shade(CameraPos.xyz, CameraNormal, UV, ltcolor, ltdir, out_Color);
}