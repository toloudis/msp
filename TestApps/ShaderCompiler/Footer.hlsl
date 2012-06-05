
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
    float4 UV		: TEXCOORD1;
    float4 diffCol	: COLOR0;
    float4 specCol	: COLOR1;
    	float3 WorldEyeDir	: TEXCOORD2;
    	float3 WorldTanMatrixX : TEXCOORD3;
    	float3 WorldTanMatrixY : TEXCOORD4;
    	float3 WorldTanMatrixZ : TEXCOORD5;
};

/*********** support functions ******/

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

vertexOutput multiLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform LightInfo LightArray[8],
    uniform float4 EyePos,
    uniform float SpecularPower,
    uniform bool LightAmbientMod,
    uniform float BumpMapScale
) 
{
    vertexOutput OUT;
    
    // Single texture layer
    OUT.TexCoord0 = mul(g_uvTransform, IN.UV);
    OUT.UV = IN.UV;
    
	float3 newPos = IN.Position;
    
    float3 Nn = mul(WorldIT, float4(IN.Normal,0)).xyz;
    Nn = normalize(Nn);    

    float4 Po = float4(newPos, 1.0);
    
    float3 Pw = mul(World, Po).xyz;
	float3 V = normalize(EyePos - Pw);

	if (g_bDoubleSided)
		Nn = faceforward(Nn, -V, Nn);
    
    OUT.diffCol = (g_emissive);
    OUT.specCol = float4(0,0,0,0);
    for (int i=0; i<6; i++)
    {
		float4 diffuse, specular;
		diffuse_contrib(LightArray[i], Pw, Nn, V, SpecularPower, diffuse, specular );
		OUT.diffCol += diffuse * g_diffuse; 
		OUT.specCol += specular * g_specular; 
    }    
    
    // Add in envmap approximation
	OUT.diffCol += envmap_approximation(g_diffuseFactor);

	OUT.diffCol.a = g_diffuse.a;
    
	getTangentToWorldSpace(World,IN.T,IN.B,IN.Normal,BumpMapScale, 
		OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);

   	// send world eye direction to pixel shader for reflection computation
	OUT.WorldEyeDir = (V);
    
    // Output vertex position
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    
    return OUT;
}
vertexOutput multiLightVS_Tess(VS_INPUT_TESS IN) 
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return multiLightVS( Vtx, g_wvp,g_worldIT,
					g_world,g_viewIT, g_lightArray, g_eyePos, g_shininess, false, g_bumpMapScale );
}

vertexOutput multiLightVS_Skin(VS_INPUT_SKINNING IN) 
{
	appdata Vtx;
	Skin(IN, Vtx);
	return multiLightVS( Vtx, g_wvp,g_worldIT,
					g_world,g_viewIT, g_lightArray, g_eyePos, g_shininess, false, g_bumpMapScale );
}
vertexOutput multiLightVS_Default(appdata Vtx) 
{
	return multiLightVS( Vtx, g_wvp,g_worldIT,
					g_world,g_viewIT, g_lightArray, g_eyePos, g_shininess, false, g_bumpMapScale );
}

perPixelVertexOutput singleLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform float BumpMapScale,
    uniform float GlowSize
) {
    perPixelVertexOutput OUT;

    OUT.TexCoord0 = mul(g_uvTransform, IN.UV);
    
	float3 newPos = IN.Position;
    
    // output position in proj space
    float4 Po = float4(newPos + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.WorldPos = Pw;

	getTangentToWorldSpace(World,IN.T,IN.B,IN.Normal,BumpMapScale, 
		OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);
	
	// decal and bump texture coords
    OUT.UV = IN.UV;
    
    return OUT;
}
perPixelVertexOutput singleLightVS_Tess(VS_INPUT_TESS IN) 
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
perPixelVertexOutput singleLightVS_Skin(VS_INPUT_SKINNING IN) 
{
	appdata Vtx;
	Skin(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
perPixelVertexOutput singleLightVS_Default(appdata Vtx) 
{
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}


pixelOutput bumpReflectPS(vertexOutput IN,
		  uniform sampler2D DiffuseMap,
		  uniform sampler2D NormalMap,
		  uniform sampler2D SpecularMap,
		  uniform sampler2D CubeMap,
		  uniform float reflectivity,
		  uniform sampler2D TransparencyMap,
		  uniform float transparency, float vFace : VFACE) 
{
	float4 refl = float4(0,0,0,0);
	if (hasCubeMap)
	{
	    //fetch bump normal
		float3 bumpNormal = Tex2DNormal(hasNormalMap, NormalMap, IN.TexCoord0);
		float3 worldNormal = normalize(float3(dot(bumpNormal,IN.WorldTanMatrixX),
			dot(bumpNormal,IN.WorldTanMatrixY),
			dot(bumpNormal,IN.WorldTanMatrixZ)));
		if (g_bDoubleSided && vFace > 0)
			worldNormal = -worldNormal;
		refl = SampleEnvironment((IN.WorldEyeDir), worldNormal,
								CubeMap);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorSampler, IN.TexCoord0, refl);
		refl *= g_reflectivity;
	}
	
    // alpha blend refl layer and base color
    float4 col = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.TexCoord0, IN.diffCol);
    float4 spec = Tex2DCombine(hasSpecularMap, SpecularMap, IN.TexCoord0, IN.specCol);

    pixelOutput OUT; 
    OUT.col.rgb = col.rgb + refl.rgb + spec;

	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.TexCoord0, transparency);
	
    return OUT;
}

pixelOutput singleLightPS(perPixelVertexOutput IN,
					uniform sampler2D DiffuseMap,
					uniform sampler2D NormalMap,
					uniform sampler2D SpecularMap,
		  			uniform sampler2D CubeMap,
					uniform LightInfo i_Light,
					uniform sampler2D SpecPowerMap,
					uniform float SpecPowerScale,
		  			uniform float reflectivity,
					uniform sampler2D TransparencyMap,
					uniform float transparency,
					float vFace : VFACE)
{
    pixelOutput OUT; 

	IncidentLight light;
	IlluminatePointLight(IN.WorldPos, i_Light, light);

	OUT.col.rgba = CustomShader( IN, light, vFace);
	
    return OUT;
}


pixelOutput projLightPS(perPixelVertexOutput IN,
		uniform sampler2D DiffuseMap,
		uniform sampler2D NormalMap,
		uniform sampler2D SpecularMap,
		uniform sampler2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform sampler2D SpecPowerMap,
		uniform float SpecPowerScale,
		uniform float reflectivity,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap,
		uniform sampler2D TransparencyMap,
		uniform float transparency,
		uniform int nBlockerSamples, 
		uniform int nShadowSamples,
		float vFace : VFACE)
{
    pixelOutput OUT; 

	IncidentLight light;
	IlluminateProjLight(IN.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);
	
	OUT.col.rgba = CustomShader( IN, light, vFace );
	
    return OUT;
}


pixelOutput glowPS(perPixelVertexOutput IN,
		uniform sampler2D NormalMap,
		uniform sampler2D SpecularMap,
		uniform sampler2D SpecPowerMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap, float vFace : VFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
    float4 mask = g_bHasMask ? tex2D(glowSampler, IN.TexCoord0) : float4(1,1,1,1);
    if (g_bConstGlow)
    {
		OUT.col = mask;
		OUT.col.a = OUT.col.r;
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.WorldPos, i_Light, light);
		
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
    
		//fetch base and specular colors
		float4 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.TexCoord0, g_specular);
		float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.TexCoord0, SpecPowerScale).r;
	        
		//fetch bump normal
		float3 bumpNormal = Tex2DNormal(hasNormalMap, NormalMap, IN.TexCoord0);
		float3 worldNormal = normalize(float3(dot(bumpNormal,IN.WorldTanMatrixX),
			dot(bumpNormal,IN.WorldTanMatrixY),
			dot(bumpNormal,IN.WorldTanMatrixZ)));
		if (g_bDoubleSided && vFace > 0)
		{
			worldNormal = -worldNormal;
		}
	        
		OUT.col.rgb = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
			light.Cls, sSpecColor, sSpecPower);
		OUT.col.a = 1;
		OUT.col *= mask;
	}
    OUT.col.a = 1;

    return OUT;
}

texture2D diffuseEnvMap;
texture2D specularEnvMap;
sampler2D diffuseEnvSampler = sampler_state
{
    Texture   = <diffuseEnvMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};
sampler2D specularEnvSampler = sampler_state
{
    Texture   = <specularEnvMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};

pixelOutput iblPS(perPixelVertexOutput IN,
		  uniform sampler2D NormalMap, float vFace : VFACE) 
{
    pixelOutput OUT; 
    
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
	float3 bumpNormal = Tex2DNormal(hasNormalMap, NormalMap, IN.TexCoord0);
	float3 worldNormal = normalize(float3(dot(bumpNormal,IN.WorldTanMatrixX),
		dot(bumpNormal,IN.WorldTanMatrixY),
		dot(bumpNormal,IN.WorldTanMatrixZ)));
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float4 refl = 0;
	if (hasCubeMap)
	{    		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeSampler , g_reflMapAngle*PI_DIV_180 , reflBlur );
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorSampler, IN.TexCoord0, refl);
	}

	float4 spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvSampler, g_specularEnvAngle);
	}
		
	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseSampler, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvSampler, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col = float4(
		g_emissive.rgb + spec.rgb + refl.rgb + diff.rgb,
		g_transparency);		

	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencySampler, IN.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif
	
    return OUT;
}

/*************/

technique Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_3_0 multiLightVS_##PassName();\
		PixelShader = compile ps_3_0 bumpReflectPS(diffuseSampler, normalSampler, specularSampler, cubeSampler, g_reflectivity, transparencySampler, g_transparency);\
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
		PixelShader = compile ps_3_0 singleLightPS(diffuseSampler, normalSampler, specularSampler, cubeSampler,\
					g_lightInfo, glossSampler, g_shininess, g_reflectivity, transparencySampler, g_transparency);\
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
		PixelShader = compile ps_3_0 projLightPS(diffuseSampler, normalSampler, specularSampler, cubeSampler,\
					g_lightInfo, g_projLight, glossSampler, g_shininess, g_reflectivity,\
						projSampler, shadowMapSampler, transparencySampler, g_transparency, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
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
		PixelShader = compile ps_3_0 projLightPS(diffuseSampler, normalSampler, specularSampler, cubeSampler,\
					g_lightInfo, g_projLight, glossSampler, g_shininess, g_reflectivity,\
						projSampler, shadowMapSampler, transparencySampler, g_transparency, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
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
		PixelShader = compile ps_3_0 projLightPS(diffuseSampler, normalSampler, specularSampler, cubeSampler,\
					g_lightInfo, g_projLight, glossSampler, g_shininess, g_reflectivity,\
						projSampler, shadowMapSampler, transparencySampler, g_transparency, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
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
		PixelShader = compile ps_3_0 projLightPS(diffuseSampler, normalSampler, specularSampler, cubeSampler,\
					g_lightInfo, g_projLight, glossSampler, g_shininess, g_reflectivity,\
						projSampler, shadowMapSampler, transparencySampler, g_transparency, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
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
		PixelShader = compile ps_3_0 glowPS(normalSampler, specularSampler, glossSampler,\
					g_lightInfo, g_projLight, g_shininess,\
					projSampler, shadowMapSampler);\
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
		VertexShader = compile vs_3_0 multiLightVS_##PassName();\
		PixelShader = compile ps_3_0 simpleMattePS();\
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
		PixelShader = compile ps_3_0 iblPS(normalSampler);\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
//PASS_ENVIRONMENT(Skin)
}
/***************************** eof ***/
