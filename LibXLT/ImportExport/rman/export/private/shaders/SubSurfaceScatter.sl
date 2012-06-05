/*****************************************************************************
**  SubSurfaceScatter.sl
**
**      SubSurfaceScatter shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// IBL()
//--------------------------------------------------------------------
color IBL( 	
	    normal Nf;
		float ss;
		float tt;
		float fresnelBias;
		float fresnelPower; 
		float g_reflMapAngle;
		float g_reflectivity;
		float g_specularPower;
		float reflBlur; 
		float g_diffuseFactor;
		float g_diffuseEnvAngle;
		color g_envDiffuseColor;
		float g_specularFactor;
		float g_specularEnvAngle;
		color g_envSpecularColor;
		string cubeMap;
		string reflectFactorTex;
		string diffuseMap;
		string diffuseEnvMap;
		string specularEnvMap;	
		string TransMapIn;
		color g_transColIn;
		float g_emissiveIntensity;
		float bRenderDiffuse;
		float bRenderSpecular;
		output varying color outRefl;
	)
{
	color outColor = 0;
	
	vector worldEyeDir = I;
	vector worldNormal = Nf;
	
	color refl = 0;
	if (cubeMap != "")
	{		
		vector incid = normalize(transform("world",I));
		vector norm = normalize(transform("world",worldNormal));
		float rmanBlur = 0.002222*reflBlur*reflBlur - 0.005*reflBlur;
		refl = SampleEnvironmentLOD(-worldEyeDir, worldNormal, cubeMap, g_reflMapAngle, rmanBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel( (-incid).norm, fresnelBias, fresnelPower);
		refl = Tex2DCombine(reflectFactorTex, refl, ss, tt);
	}

	outRefl = refl;

	color spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if ( specularEnvMap != "" )
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, specularEnvMap, g_specularEnvAngle);
	}
	
	color transColMapIn = Tex2DCombine(TransMapIn, g_transColIn, ss, tt);
	color diff = Tex2DCombine(diffuseMap, transColMapIn*g_envDiffuseColor*g_diffuseFactor, ss, tt);
	if ( diffuseEnvMap != "" )
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
	}
	
	outColor = color( spec*bRenderSpecular + refl + diff*bRenderDiffuse );

	return outColor;
}

//--------------------------------------------------------------------
// TransPass()
//--------------------------------------------------------------------
color TransPass( vector DotLN; float ss; float tt; string TransSampler; 
				  float g_transRampOff; float g_transMultiplier;
				  string TransMapInSampler; color g_transColIn;
				  string TransMapOutSampler; color g_transColOut;
				  string TransMapBackSampler; color g_transColBack)
{

	color transSample = Tex2DReplace(TransSampler, color(0,0,0), ss, tt);

	color DotLNCol = color(DotLN);

	float TranslucenceX	= smoothstep( -g_transRampOff*(transSample[0]), 1, abs( xcomp(DotLN) ) );
	float TranslucenceY	= smoothstep( -g_transRampOff*(transSample[1]), 1, abs( ycomp(DotLN) ) );
	float TranslucenceZ	= smoothstep( -g_transRampOff*(transSample[2]), 1, abs( zcomp(DotLN) ) );

	color Translucence = color( TranslucenceX, TranslucenceY, TranslucenceZ );

	color transColMapIn = Tex2DCombine(TransMapInSampler, g_transColIn, ss, tt);
	color transColMapOut = Tex2DCombine(TransMapOutSampler, g_transColOut, ss, tt);
	color transColMapBack = Tex2DCombine(TransMapBackSampler, g_transColBack, ss, tt);
	
	color preColourise = lerp(transColMapOut * g_transMultiplier,transColMapIn,DotLNCol);

	color Colourise	= lerp(transColMapBack, preColourise ,Translucence);
	
	return (Colourise * Translucence);

}

//--------------------------------------------------------------------
// FresnelMask()
//--------------------------------------------------------------------
float FresnelMask( vector Normals; vector EyeVec; float g_specFresnel )
{
	float Fresnel = EyeVec.Normals * g_specFresnel;
	return saturate(Fresnel);
}

//--------------------------------------------------------------------
// SpecularFrontOn()
//--------------------------------------------------------------------
color SpecularFrontOn(	vector Normals;
						vector EyeVec;
						vector LightVec;
						float Power;
						float Gloss)
{
	vector SpecReflect = (2 * Normals.LightVec * Normals - LightVec);
	float Specular = pow( saturate(SpecReflect.EyeVec) , max(0.001, Gloss) ) * Power;
	return color(Specular);
}

//--------------------------------------------------------------------
// SkinShader()
//--------------------------------------------------------------------
color SkinShader(float ss; float tt; vector WN; vector EV; vector LV; 
				  color LightColourDiff; color LightColourSpec; 
				  float diffuseFactor; string TransSampler; 
				  float g_transRampOff; float g_transMultiplier;
				  string TransMapInSampler; color g_transColIn;
				  string TransMapOutSampler; color g_transColOut;
				  string TransMapBackSampler; color g_transColBack;
				  string DiffSampler; float g_specFresnel; float g_fresnelPower;
				  float g_fresnelGloss; string SpecSampler; string SpecPowerSampler;
				  float g_specPower; float g_specGloss; color g_specColor;
				  float bEnableDiffuse; float bEnableSpecular)
{

	vector DotLN = LV.WN;

	color Translucency	= TransPass(DotLN,ss,tt,TransSampler, 
									g_transRampOff, g_transMultiplier,
									TransMapInSampler, g_transColIn,
									TransMapOutSampler, g_transColOut,
									TransMapBackSampler, g_transColBack);

    color a = Tex2DReplace(DiffSampler, color(1,1,1), ss, tt);
	color b = Translucency;

	color BaseLighting = (a * b)*(a + b + b - 2*a*b);

	float fresnelMask = FresnelMask(WN,EV,g_specFresnel);
	float FresnelStrength = saturate(1 - fresnelMask) * g_fresnelPower + 1;
	float FresnelGlossiness	= fresnelMask * g_fresnelGloss + 1;
	color specTexSample = Tex2DReplace(SpecSampler, color(1,1,1), ss, tt);

	color specPowerTexSampleCol = Tex2DReplace(SpecPowerSampler, color(1,1,1), ss, tt);
	float specPowerTexSample = specPowerTexSampleCol[0];

	color Spec = SpecularFrontOn(WN,EV,LV, 
				(g_specPower * specPowerTexSample * FresnelStrength) , 
				(g_specGloss * FresnelGlossiness * max(0.1,1)) )
				* g_specColor * specTexSample * Translucency[0];

	color LightingOutput = BaseLighting * LightColourDiff * diffuseFactor * bEnableDiffuse  + Spec * LightColourSpec * bEnableSpecular;
				
	return LightingOutput;
} 

//--------------------------------------------------------------------
// SubSurfaceScatter()
//--------------------------------------------------------------------
surface 
SubSurfaceScatter(
		string cubeTex = "";
		string diffTex = "";
		string microTex = "";
		string reflectFactorTex = "";
		string specPowerTex = "";
		string specTex = "";
		string transMapBack = "";
		string transMapIn = "";
		string transMapOut = "";
		string transTex = "";
		string transparencyTex = "";
		color g_specColor = 0.5;
		color g_transColBack = 0.5;
		color g_transColIn = 0.5;
		color g_transColOut = 0.5;
		float fresnelBias = 0.2;
		float fresnelPower = 4;
		float g_fresnelGloss = 0;
		float g_fresnelPower = 15;
		float g_microScale = 50;
		float g_reflMapAngle = 0;
		float g_reflectivity = 1;
		float g_specFresnel = 3;
		float g_specGloss = 15;
		float g_specPower = 0.2;
		float g_transMultiplier = 1;
		float g_transRampOff = 1;
		float g_transparency = 1; 
		float reflBlur = 0;
		string diffuseEnvMap = "";
		string specularEnvMap = "";	
		float g_diffuseFactor = 1;
		float g_diffuseEnvAngle = 0;
		color g_envDiffuseColor = color(0.5,0.5,0.5);
		float g_specularFactor = 1;
		float g_specularEnvAngle = 0;
		color g_envSpecularColor = color(0,0,0);
		float g_emissiveIntensity = 1;
		float UScale = 1;
		float VScale = 1;
		float UTrans = 0;
		float VTrans = 0;
		float UVAngle = 0;
		string normalMap = "";
		float normalMapScale = 1;
		float g_DisplacementEnabled = 0;
		string displacementMap = "";
		float g_displacementScale = 0;
		float g_displacementBias = 0;
		float g_displacementBlur = 0;
		float g_DisplacementNormalScale = 1;
		float bRenderEnvironments = 1;
		float bRenderLit = 1;
		float bRenderDiffuse = 1;
		float bRenderSpecular = 1;
		float bRenderTransparent = 1;
		float g_isPlanar = 0;
		float dynamicReflection = 0;
		float objVisible = 1;
		float bRenderAO = 1;
		float AOSamples = 256;
		float AOMaxDist = 1000;
		float AOMaxVariation = 0.1;
		float AOConeAngle = 90;
		color AOColor = 0;
		float AORadiusNear = 10;
		float AORadiusFar = 10;
		float AOAngleBias = 10;
		float AOAttenuation = 10;
		float AOContrast = 10;
		float AOBlurWidth = 10;
		float AOBlurSharpness = 10;
		float AOOverscanPixels = 10;
		float camNearClip = 1;
		float camFarClip = 10000;
		float bRenderGI = 1;
		float GISamples = 256;
		float GIMaxDist = 1000;
		float GIMaxVariation = 0.1;
		float GIConeAngle = 90;
		color GIColor = 0;
		float GIRadiusNear = 10;
		float GIRadiusFar = 10;
		float GIAngleBias = 10;
		float GIAttenuation = 10;
		float GIContrast = 10;
		float GIBlurWidth = 10;
		float GIBlurSharpness = 10;
		float GIOverscanPixels = 10;
		float g_IsolateReflection = 0;
		output float bReceivesShadow = 1;
		output varying color outGI = color(0,0,0);
		output varying color outRefl = color(0,0,0);

		// Dummy vars for warning removal
		float g_bFlatTessellate = 0;
		string g_DisplacementMap = "";
		float g_DisplacementScale = 0;
		float g_DisplacementBias = 0;
		float g_DisplacementBlur = 0;
	)
{
	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	normal Nf = GetBumpNormal(normalMap, normalMapScale, ss, tt);

	color C = 0;
		
	if ( bRenderEnvironments == 1 )
	{
		C = IBL(
			Nf,
			ss,
			tt,
			fresnelBias,
			fresnelPower,
			g_reflMapAngle,
			g_reflectivity,
			g_specPower,
			reflBlur,
			g_diffuseFactor,
			g_diffuseEnvAngle,
			g_envDiffuseColor,
			g_specularFactor,
			g_specularEnvAngle,
			g_envSpecularColor,
			cubeTex,
			reflectFactorTex,
			diffTex,
			diffuseEnvMap,
			specularEnvMap,
			transMapIn,
			g_transColIn,
			g_emissiveIntensity,
			bRenderDiffuse,
			bRenderSpecular,
			outRefl);
	}
	
	illuminance( P ) {

		// Get light source info
		float bEnableLight = 1;
		float bEnableDiffuse = 1;
		float bEnableSpecular = 1;
		float bAffectsGlow = 1;
		lightsource("bEnableLight",bEnableLight);
		lightsource("bEnableDiffuse",bEnableDiffuse);
		lightsource("bEnableSpecular",bEnableSpecular);
		lightsource("bAffectsGlow",bAffectsGlow);
		bEnableDiffuse *= bRenderDiffuse;
		bEnableSpecular *= bRenderSpecular;
		color Cld = Cl * bRenderDiffuse;
		color Cls = Cl * bRenderSpecular;
		
		// Calculate color
		color outColor = SkinShader( ss, tt, Nf, -normalize(I), normalize(L), 
									 Cld, Cls, 
									 g_diffuseFactor, transTex, 
									 g_transRampOff, g_transMultiplier,
									 transMapIn, g_transColIn,
									 transMapOut, g_transColOut,
									 transMapBack, g_transColBack,
									 diffTex, g_specFresnel, g_fresnelPower,
									 g_fresnelGloss, specTex, specPowerTex,
									 g_specPower, g_specGloss, g_specColor,
									 bEnableDiffuse, bEnableSpecular );

		C += outColor * bEnableLight * bRenderLit;
	
	}
	
	CalculateGIAO(C, P, Nf, camNearClip, camFarClip, 
				  GISamples, GIMaxVariation, GIMaxDist, GIConeAngle, bRenderGI,
				  GIColor, GIRadiusNear, GIRadiusFar, GIAngleBias, GIAttenuation, GIContrast,
				  AOSamples, AOMaxVariation, AOMaxDist, AOConeAngle, bRenderAO,
				  AOColor, AORadiusNear, AORadiusFar, AOAngleBias, AOAttenuation, AOContrast, 
				  outGI 
				  );
	
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;
		
	Oi = Tex2DCombineOpacity( transparencyTex , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;
	Oi = Tex2DCombineOpacity( transparencyTex , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;
	Ci = Oi * Cs * C;
}







