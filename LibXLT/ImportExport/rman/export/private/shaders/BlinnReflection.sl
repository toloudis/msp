/*****************************************************************************
**  BlinnReflection.sl
**
**      BlinnReflection shader
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
		color g_emissive;
		color g_specular;
		float fresnelBias;
		float fresnelPower; 
		float g_reflectivity;
		float g_shininess;
		float g_specularPower;
		float g_transparency;
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
		string normalMap;
		float normalMapScale;
		string reflectSmearMap;
		float smearMapScale;
		float g_DisplacementEnabled;
		float g_DisplacementNormalScale;
		float g_isPlanar;
		float dynamicReflection;
		float refrFactor;
		float reflFactor;
		float IOR;
		float bRenderDiffuse;
		float bRenderSpecular;
		string planarSampler;
		float reflLOD;
		float reflEnable;
		float reflType;		
		float g_IsolateReflection;
		output varying color outRefl;
	)
{
	color outColor = 0;
	
	vector worldNormal = Nf;
	
	float bumpScale = 1.0;

    if (normalMap != "")
		bumpScale = normalMapScale;

	color rr = 0;

	vector bumpNormal = Tex2DNormal(normalMap, ss, tt) * vector( normalMapScale, normalMapScale, 1 );
	if (reflectSmearMap != "")
	{
		bumpNormal = Tex2DNormal(reflectSmearMap, ss, tt);
		worldNormal = GetBumpNormal(reflectSmearMap, smearMapScale, ss, tt);
		bumpScale = smearMapScale;
	}
	
	rr = GetReflection( ss, tt, bumpNormal, 
						worldNormal, P, I, 
						bumpScale, refrFactor, reflFactor,
						reflectFactorMap, g_isPlanar,
						fresnelBias, fresnelPower, IOR, 
						cubeMap, planarSampler, reflLOD,
						reflEnable, reflType);	

	outRefl = rr;

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

	if ( g_IsolateReflection == 1 )
	{
		outColor = rr;
	}
	else
	{	
		outColor = color( g_emissive*g_emissiveIntensity + spec*bRenderSpecular + rr + diff*bRenderDiffuse );
	}

	return outColor;
}

//--------------------------------------------------------------------
// BlinnReflection()
//--------------------------------------------------------------------
surface 
BlinnReflection(	
		string diffuseMap = "";
		string glossMap = "";
		string iorMap = "";
		string reflectFactorMap = "";
		string reflectSmearMap = "";
		string specularMap = "";
		string transparencyMap = "";
		color g_diffuse = 0.5; 
		color g_emissive = 0;
		color g_specular = 1;
		float IOR = 1;
		float fresnelBias = 0.2;
		float fresnelPower = 4;
		float g_IOR = 1;
		float g_emissiveIntensity = 1;
		float g_reflectivity = 1;
		float g_roughness = 0;
		float g_shininess = 20;
		float g_specularPower = 1;
		float g_transparency = 1;
		float reflFactor = 1;
		float reflLOD = 0;
		float refrFactor = 0;
		float refrLOD = 0;
		float smearMapScale = 0; 
		float g_diffuseFactor = 1;
		float g_diffuseEnvAngle = 0;
		color g_envDiffuseColor = color(0.5,0.5,0.5);
		float g_specularFactor = 1;
		float g_specularEnvAngle = 0;
		color g_envSpecularColor = color(0,0,0);
		color g_ambient = color(0,0,0);
		string cubeMap = "";
		string diffuseEnvMap = "";
		string specularEnvMap = "";
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
		string planarSampler = "";
		float reflEnable = 0;
		float reflType = 0;
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
	vector V = -normalize(I);

	color C = 0;
		
	if ( bRenderEnvironments == 1 )
	{
		C = IBL( 
				Nf,
				ss,
				tt,
				g_diffuse,
				g_emissive,
				g_specular,
				fresnelBias,
				fresnelPower,
				g_reflectivity,
				g_shininess,
				g_specularPower,
				g_transparency,
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
				normalMap,
				normalMapScale,
				reflectSmearMap,
				smearMapScale,
				g_DisplacementEnabled,
				g_DisplacementNormalScale,
				g_isPlanar,
				dynamicReflection,
				refrFactor,
				reflFactor,
				IOR,
				bRenderDiffuse,
				bRenderSpecular,
				planarSampler,
				reflLOD,
				reflEnable,
				reflType,
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

		// map 0..1 to 1..g_IOR 
		color ior_color = Tex2DCombine( iorMap, color(g_IOR-1,g_IOR-1,g_IOR-1), ss, tt );
		float ior = ior_color[0] + 1;
		float g_specularBias = 0;

		color sDiffColor = Tex2DCombine( diffuseMap , g_diffuse, ss, tt );
		color sSpecColor = Tex2DCombine( specularMap , g_specular, ss, tt );
		color sSpecPower = Tex2DCombine( glossMap , color(g_shininess), ss, tt );

		// Calculate color
		color diffuse = PhongDiffuse(Nf, normalize(L), Cld, sDiffColor) * bEnableDiffuse;
		if (g_roughness>0)
		{
			float oren = OrenNayarDiffuse(normalize(L), normalize(V), Nf, g_roughness);
			diffuse *= oren;
		}
		color specular = BlinnSpecular(Nf, normalize(L), normalize(V), Cls, sSpecColor, sSpecPower[0], ior, g_specularBias,g_specularPower) * bEnableSpecular;

		color outColor = (diffuse + specular) * bEnableLight * bRenderLit;

		C += outColor;
	}

	CalculateGIAO(C, P, Nf, camNearClip, camFarClip, 
				  GISamples, GIMaxVariation, GIMaxDist, GIConeAngle, bRenderGI,
				  GIColor, GIRadiusNear, GIRadiusFar, GIAngleBias, GIAttenuation, GIContrast,
				  AOSamples, AOMaxVariation, AOMaxDist, AOConeAngle, bRenderAO,
				  AOColor, AORadiusNear, AORadiusFar, AOAngleBias, AOAttenuation, AOContrast, 
				  outGI 
				  );
	
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;
		
	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;
	Ci = Oi * Cs * C;
}







