/*****************************************************************************
**  Phong.sl
**
**      Phong without tangent space normal mapping
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
		color g_diffuse;
		color specularcolor;
		color g_emissive;
		color g_specular;
		float fresnelBias;
		float fresnelPower; 
		float g_reflMapAngle;
		float g_reflectivity;
		float g_shininess;
		float g_specularPower;
		float g_transparency;
		float reflBlur; 
		float g_diffuseFactor;
		float g_diffuseEnvAngle;
		color g_envDiffuseColor;
		float g_specularFactor;
		float g_specularEnvAngle;		
		color g_envSpecularColor;
		string cubeMap;
		string diffuseMap;
		string glossMap;
		string reflectFactorMap; 
		string specularMap; 
		string transparencyMap;
		string diffuseEnvMap;
		string specularEnvMap;
		float g_emissiveIntensity;
		float bRenderDiffuse;
		float bRenderSpecular;
		float g_IsolateReflection;
		output varying color outRefl;
	)
{
	color outColor = 0;
	
	vector worldNormal = Nf;
	
	color refl = 0;
	if (cubeMap != "")
	{
		vector incid = normalize(transform("world",I));
		vector norm = normalize(transform("world",worldNormal));
		float rmanBlur = 0.002222*reflBlur*reflBlur - 0.005*reflBlur;
		refl = SampleEnvironmentLOD(-I, worldNormal, cubeMap, g_reflMapAngle, rmanBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel( -incid.norm, fresnelBias, fresnelPower);
		refl = Tex2DCombine(reflectFactorMap, refl,ss,tt);
	}

	color spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if ( specularEnvMap != "" )
	{
		spec *= SampleEnvironment((-I), worldNormal, specularEnvMap, g_specularEnvAngle);
	}
	
	color diff = Tex2DCombine(diffuseMap, g_diffuse*g_envDiffuseColor*g_diffuseFactor, ss, tt);
	if ( diffuseEnvMap != "" )
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
	}

	outRefl = refl;
	
	outColor = color( g_emissive*g_emissiveIntensity + spec*bRenderSpecular + refl + diff*bRenderDiffuse );

	return outColor;
}

//--------------------------------------------------------------------
// Phong()
//--------------------------------------------------------------------
surface 
Phong( 	
		color g_diffuse = 0.5;
		color specularcolor = 1;
		color g_emissive = 0.5;
		color g_specular = 0.5;
		float fresnelBias = 0.2;
		float fresnelPower = 4; 
		float g_reflMapAngle = 0;
		float g_reflectivity = 1;
		float g_shininess = 1;
		float g_specularPower = 1;
		float g_transparency = 1;
		float reflBlur = 0; 
		float g_diffuseFactor = 1;
		float g_diffuseEnvAngle = 0;
		color g_envDiffuseColor = color(0.5,0.5,0.5);
		float g_specularFactor = 1;
		float g_specularEnvAngle = 0;
		color g_envSpecularColor = color(0,0,0);
		color g_ambient = color(0,0,0);
		string cubeMap = "";
		string diffuseMap = "";
		string glossMap = "";
		string reflectFactorMap = "";
		string specularMap = "";
		string transparencyMap = "";
		string diffuseEnvMap = "";
		string specularEnvMap = "";
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
		float g_IsolateReflection = 0;
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
				g_diffuse,
				specularcolor,
				g_emissive,
				g_specular,
				fresnelBias,
				fresnelPower,
				g_reflMapAngle,
				g_reflectivity,
				g_shininess,
				g_specularPower,
				g_transparency,
				reflBlur,
				g_diffuseFactor,
				g_diffuseEnvAngle,
				g_envDiffuseColor,
				g_specularFactor,
				g_specularEnvAngle,
				g_envSpecularColor,
				cubeMap,
				diffuseMap,
				glossMap,
				reflectFactorMap,
				specularMap,
				transparencyMap,
				diffuseEnvMap,
				specularEnvMap,
				g_emissiveIntensity,
				bRenderDiffuse,
				bRenderSpecular,
				g_IsolateReflection,
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
		color sDiffColor = Tex2DCombine( diffuseMap , g_diffuse, ss, tt );
		color sSpecColor = Tex2DCombine( specularMap , g_specular, ss, tt );
		color sSpecPower = Tex2DCombine( glossMap , color(g_shininess), ss, tt );

		color diffuse = PhongDiffuse(Nf, normalize(L), Cld, sDiffColor) * bEnableDiffuse;
		color specular = PhongSpecular(Nf, normalize(L), normalize(-I), Cls, sSpecColor, sSpecPower[0]) * g_specularPower * bEnableSpecular;
		
		color outColor = (diffuse + specular) * bEnableLight * bRenderLit;

		C += outColor;
	}

	CalculateGIAO(C, P, Nf, camNearClip, camFarClip, 
				  GISamples, GIMaxVariation, GIMaxDist, GIConeAngle, bRenderGI,
				  GIColor, GIRadiusNear, GIRadiusFar, GIAngleBias, GIAttenuation, GIContrast,
				  AOSamples, AOMaxVariation, AOMaxDist, AOConeAngle, bRenderAO,
				  AOColor, AORadiusNear, AORadiusFar, AOAngleBias, AOAttenuation, AOContrast, 
				  outGI);

	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;

	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;

	Ci = Oi * Cs * C;
}







