
/*****************************************************************************
/* FOOTER
/****************************************************************************/
/* data from application vertex buffer */
struct appdata {
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float4 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float4 UV			: TEXCOORD1;
    float4 diffCol		: COLOR0;
    float4 specCol		: COLOR1;
    float3 WorldEyeDir	: TEXCOORD2;
    float3 WorldTanMatrixX : TEXCOORD3;
    float3 WorldTanMatrixY : TEXCOORD4;
    float3 WorldTanMatrixZ : TEXCOORD5;
};

sampler2D diffuseEnvSampler = sampler_state
{
    Texture   = <diffuseEnvMap>;
    MipFilter = LINEAR;
    MinFilter = Anisotropic;
    MaxAnisotropy = 16;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};
sampler2D specularEnvSampler = sampler_state
{
    Texture   = <specularEnvMap>;
    MipFilter = LINEAR;
    MinFilter = Anisotropic;
    MaxAnisotropy = 16;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};

/*********** support functions ******/
void IlluminateEnvLight(in float3 worldNormal, in float3 Ps, out IncidentLight OUT)
{
	float3 worldEyeDir = normalize(Ps - g_eyePos.xyz);	

	float4 spec = g_envSpecularColor*g_specularFactor; 
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvSampler, g_specularEnvAngle);
	}

	OUT.Cls = spec.rgb;
		
	float4 diff = g_envDiffuseColor*g_diffuseFactor;
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvSampler, g_diffuseEnvAngle).rgb,1);
	}

	OUT.Cld = diff.rgb;

	//OUT.L = light.Pos.xyz - Ps;
	OUT.L = worldNormal;
}

/*********** vertex shader ******/
DOFvertexOutput DOFPrep_VS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldView)
{
    DOFvertexOutput OUT; 
    // output position in proj space
	float4 Po = float4(IN.Position, 1.0f);
    OUT.HPosition = mul(WorldViewProj, Po);
    OUT.ViewSpacePos = mul(WorldView, Po);
    return OUT;
}
DOFvertexOutput DOFPrep_VS_Tess(VS_INPUT_TESS IN)
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Skin(VS_INPUT_SKINNING IN)
{
	appdata Vtx;
	Skin(IN, Vtx);
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Default(appdata Vtx)
{
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}

TANGENT_VERTEX_OUTPUT singleLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform float BumpMapScale,
    uniform float GlowSize
) {
    TANGENT_VERTEX_OUTPUT OUT;

    OUT.V.TexCoord0 = mul(g_uvTransform, IN.UV);
    
	float3 newPos = IN.Position;

    // output position in proj space
    float4 Po = float4(newPos + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.V.WorldPos = Pw;

	OUT.V.WorldTan = TransformTangents( World, IN.T, IN.B, IN.Normal );

	// decal and bump texture coords
    OUT.V.UV = IN.UV;

	OUT.ScreenPos = float3(0,0,0);//not used

    return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Tess(VS_INPUT_TESS IN) 
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Skin(VS_INPUT_SKINNING IN) 
{
	appdata Vtx;
	Skin(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Default(appdata Vtx) 
{
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform sampler2D NormalMap,
					uniform LightInfo i_Light,
					float vFace : VFACE)
{
    pixelOutput OUT; 

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
	float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	float3 L = normalize(light.L);
	float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 E = g_eyePos.xyz;
	float3 P = IN.V.WorldPos;
	float3 Cld = light.Cld;
	float3 Cls = light.Cls;
	
	OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
	
    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
								uniform sampler2D NormalMap,
								uniform LightInfo i_Light,
							   float vFace : VFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput singleLightPS_Skin( TANGENT_VERTEX_OUTPUT IN,
								uniform sampler2D NormalMap,
								uniform LightInfo i_Light,
							   float vFace : VFACE )
{
	return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,								  
								uniform sampler2D NormalMap,							
								uniform LightInfo i_Light,							
								  float vFace : VFACE )
{
	return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform sampler2D NormalMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform int nBlockerSamples, 
		uniform int nShadowSamples,
		float vFace : VFACE)
{
    pixelOutput OUT; 

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, projSampler, shadowMapSampler, nBlockerSamples, nShadowSamples, light);
	
	float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
	float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	float3 L = normalize(light.L);
	float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 E = g_eyePos.xyz;
	float3 P = IN.V.WorldPos;
	float3 Cld = light.Cld;
	float3 Cls = light.Cls;
	
	OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
	
    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							uniform sampler2D NormalMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
							 float vFace : VFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Skin( TANGENT_VERTEX_OUTPUT IN,
							uniform sampler2D NormalMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
							 float vFace : VFACE )
{
	return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform sampler2D NormalMap,
								uniform LightInfo i_Light,
								uniform ProjLightInfo i_ProjLight,
								uniform int nBlockerSamples, uniform int nShadowSamples, 
								float vFace : VFACE )
{
	return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform sampler2D NormalMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		float vFace : VFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
    float4 mask = g_bHasMask ? tex2D(glowSampler, IN.V.TexCoord0) : float4(1,1,1,1);
    if (g_bConstGlow)
    {
		OUT.col = mask;
		OUT.col.a = OUT.col.r;
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, projSampler, shadowMapSampler, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.V.WorldPos, i_Light, light);
				
		float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
		float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
		float3 L = normalize(light.L);
		float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
		float3 E = g_eyePos.xyz;
		float3 P = IN.V.WorldPos;
		float3 Cld = float3(0,0,0);
		float3 Cls = light.Cls;
		
		OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
		
		OUT.col *= mask;
		
	}
    OUT.col.a = 1;    

    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform sampler2D NormalMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						float vFace : VFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}

pixelOutput glowPS_Skin( TANGENT_VERTEX_OUTPUT IN,
						uniform sampler2D NormalMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						float vFace : VFACE ) 
{
	return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform sampler2D NormalMap, 
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
						   float vFace : VFACE )
{
	return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}


pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
		  uniform sampler2D NormalMap, float vFace : VFACE) 
{
    pixelOutput OUT;     
    IncidentLight light;    
    
    float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
    
	IlluminateEnvLight( N , IN.V.WorldPos, light);
		
	float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);	
	float3 L = normalize(light.L);
	float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 E = g_eyePos.xyz;
	float3 P = IN.V.WorldPos;
	float3 Cld = light.Cld;
	float3 Cls = light.Cls;
	
	OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 	

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, NormalMap, vFace );
}

pixelOutput iblPS_Skin( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
	return iblPS( IN, NormalMap, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
	return iblPS( IN, NormalMap, vFace );
}
/*************/

technique Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 singleLightPS_##PassName(normalSampler, g_lightInfo);\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
//PASS_DEFAULT(Skin)
}

technique SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 singleLightPS_##PassName(normalSampler, g_lightInfo);\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
//PASS_SINGLELIGHT(Skin)
}

technique ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 projLightPS_##PassName(normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
//PASS_PROJECTEDLIGHT(Skin)
}
technique ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 projLightPS_##PassName(normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
//PASS_PROJECTEDLIGHTSS(Skin)
}
technique ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 projLightPS_##PassName( normalSampler,	g_lightInfo, g_projLight, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
	}
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
//PASS_PROJECTEDLIGHTSS2(Skin)
}
technique ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 projLightPS_##PassName( normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
	}
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
//PASS_PROJECTEDLIGHTSS3(Skin)
}

technique Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 glowPS_##PassName(normalSampler, g_lightInfo, g_projLight);\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
//PASS_GLOW(Skin)
}
technique DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 DOFPrep_VS_##PassName();\
		PixelShader = compile ps_3_0 DOFPrep_PS();\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
//PASS_DOFPREP(Skin)
}
technique Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 simpleMattePS(); \
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
//PASS_MATTE(Skin)
}
technique Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 singleLightVS_##PassName();\
		PixelShader = compile ps_3_0 iblPS_##PassName(normalSampler);\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
//PASS_ENVIRONMENT(Skin)
}
/***************************** eof ***/
