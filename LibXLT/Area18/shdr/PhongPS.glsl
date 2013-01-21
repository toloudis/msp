#include "ShaderSDK.hlsl"

cbuffer cbMaterial
{
	float4 g_Color = {1,1,1,1};
};

float4 Shade(IncidentLight i_IncidentLight, float3 Ng, float3 L, float3 E, float3 P)
{
	float3 c = saturate(dot(Ng,L)) * i_IncidentLight.m_Color * g_Color.rgb;
	return float4(c,1);
}

float4 PS( SHADE_POINT In ) : SV_Target
{
	float3 N = normalize(In.ViewTan.Z);
	float3 L = normalize(g_LightPos - In.ViewPos);
	float3 E = normalize(g_EyePos.xyz - In.ViewPos);
	float3 P = In.ViewPos;

	IncidentLight incidentLight;
	IlluminatePointLight(incidentLight);

	return Shade(incidentLight, N, L, E, P);
}

