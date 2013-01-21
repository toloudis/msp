#define SPOT 0
#define POINT 1

#define LIGHT POINT

struct IncidentLight
{
	// direction, unnormalized
	float3 L;
	// color
	float3 Cld;
	float3 Cls;
};

#if LIGHT == POINT
cbuffer cbLight 
{
	float4 g_Pos = {0,0,-1,1};
	float4 g_Diffuse = {1,1,1,1};
	float4 g_Specular = {1,1,1,1};
};

void Illuminate(in float3 Ps, out IncidentLight OUT)
{
	OUT.L = g_Pos.xyz - Ps;
	OUT.Cld = g_Diffuse.rgb;
	OUT.Cls = g_Specular.rgb;
}

#elif LIGHT == SPOT
// spot light 

cbuffer cbLight 
{
	float4 g_Pos = {0,0,-1,1};
	float4 g_Dir = {0,0,1,0};
	float4 g_Diffuse = {1,1,1,1};
	float4 g_Specular = {1,1,1,1};
	// view space into light space
	float4x4 g_Matrix;
	foat2 g_NearFar;
	float g_InnerAngle;
	float g_OuterAngle;
	bool g_bHasProjMap = false;
	bool g_bHasShadowMap = false;
};

struct IncidentLight
{
	// direction, unnormalized
	float3 L;
	// color
	float3 Cld;
	float3 Cls;
};

Texture2D projLightMap;
SamplerState projSampler
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(0,0,0,0);
};

Texture2D projShadowMap;
SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1,1,1,1);
};

float EllipticalPenumbra(float i_InnerAngle, float i_OuterAngle, float i_Aspect, float2 i_UV)
{
	// this is based on ellipses, see ARman section 14.3.1.
	float a = 0.5 * tan(i_InnerAngle*0.5)/tan(i_OuterAngle*0.5);
	float A = 0.5;
	float b = 0.5 * tan(i_Aspect*i_InnerAngle*0.5)/tan(i_Aspect*i_OuterAngle*0.5);
	float B = 0.5;
	float2 xy = abs(i_UV-0.5);
	float q = a*b/sqrt(b*b*xy.x*xy.x + a*a*xy.y*xy.y);
	float r = A*B/sqrt(B*B*xy.x*xy.x + A*A*xy.y*xy.y);
	return 1-smoothstep(q,r,1);
//			float3 lvec = -normalize(OUT.L);
//			float3 ldir = -(light.ConeInfo.xyz);
//			return (dot(lvec, ldir) - cos(projLight.OuterAngle*0.5)) / (cos(projLight.InnerAngle*0.5) - cos(projLight.OuterAngle*0.5));
}

//--------------------------------------------------------------------
//	IlluminateProjLight() - projected light with shadow
//--------------------------------------------------------------------
void Illuminate(in float3 Ps, out IncidentLight OUT)
{
	OUT.L = g_Pos.xyz - Ps;
	if (g_Pos.w == 0) // directional projected
		OUT.L = g_Dir.xyz;

	OUT.Cld = OUT.Cls = float3(0,0,0);
	float4 projTexCoord = mul(g_Matrix, float4(Ps,1));
	if (projTexCoord.z >= 0 && projTexCoord.z < g_NearFar.y)
	{
		float4 tex_col = 1;
		if( g_bHasProjMap )
		{
			tex_col = projLightMap.SampleLevel(projSampler, projTexCoord.xy/projTexCoord.w, 0);//tex2Dproj(ProjTextureMap, projTexCoord); 
		}

		float2 uv = projTexCoord.xy/projTexCoord.w;

		if (g_InnerAngle <= g_OuterAngle)
		{
			float p = EllipticalPenumbra(g_InnerAngle, 
				g_OuterAngle, g_Aspect, uv);
			tex_col *= p;
		}
		else
		{
			uv = step(0, uv) * step(uv, 1);
			tex_col *= uv.x * uv.y;
		}

		float atten = 1;//attenuation(Ps, light);

		float4 shadowCoeff = 1;
//		if( g_bHasShadowMap )
//		{
//			shadowCoeff = PCSS(ProjShadowMap, projTexCoord, nBlockerSamples, nShadowSamples);
//			//shadowCoeff = SampleShadowMap(ProjShadowMap, projTexCoord, nShadowSamples, g_projLight.LightSize);
//			//shadowCoeff = CalculateVarianceShadow(ProjShadowMap, projTexCoord);
//		}			

		// multiply texture color with shadow color
		float3 modifier = g_ShadowColor.rgb * atten * tex_col.rgb;
		// OUT.Cld = modifier * light.Diffuse.rgb;
		// OUT.Cls = modifier * light.Specular.rgb;
		OUT.Cld = lerp( modifier, atten * tex_col.rgb * g_Diffuse.rgb, shadowCoeff.rgb);
		OUT.Cls = lerp( modifier, atten * tex_col.rgb * g_Specular.rgb, shadowCoeff.rgb);
	}
}


#endif