float4 g_diffuse : MaterialDiffuse	// multiLightVS, ibl
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "diffuse color of surface";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float4 g_specular : MaterialSpecular	// multiLightVS
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 2;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float g_shininess : MaterialPower	// multiLightVS
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 15.0;
	float SasUiMax = 1000.0;
	int UiIndex = 3;
> = 20.0f;

float3 MyDiffuse(float3 normal, float3 lightDir, float3 lDiffColor, float3 sDiffColor)
{
	float cosine = saturate(dot(normal,lightDir));
	return cosine * (lDiffColor * sDiffColor);
}

float3 MySpecular(float3 normal, float3 lightDir, float3 eyeDir, 
	float3 lSpecColor, float3 sSpecColor, float sSpecPower)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light

	float specComp = saturate(dot(normal,normalize(lightDir + eyeDir)));
    specComp = (dot(normal, lightDir)>0) * (pow(specComp, max(0.001, sSpecPower)));
	return specComp * (lSpecColor * sSpecColor);
}


/*****************************************************************************
/* SurfaceShader()
/*
/* ShaderCompiler replaces "SurfaceShader()" with this function stub:
/****************************************************************************/
float4 SurfaceShader(float3 Ng, float3 N, float3 L, float3 I, float3 E, float3 P, float3 Cld, float3 Cls, float4 Csd, float4 Css, float Os, float g_bumpMapScale)
//float4 SurfaceShader()
{

	float3 diff = MyDiffuse( N, L, Cld, g_diffuse );
	float3 spec = MySpecular( N, L, -I, Cls, g_specular , g_shininess );

	// Figure out color however you like
	return float4( diff + spec, 1 );
}
