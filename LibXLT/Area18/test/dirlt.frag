#version 410

// uniform inputs
uniform vec4 ltColor;
uniform vec3 ltDir;
uniform mat4 ltModelView;
uniform mat4 ltProjection;

void light(vec3 P, vec3 n, vec2 uv, out vec4 ocolor, out vec3 incidentDir)
{
	ocolor = ltColor;
	incidentDir = normalize(ltDir);
}
