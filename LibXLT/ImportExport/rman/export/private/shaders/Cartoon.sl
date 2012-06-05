/*****************************************************************************
**  Cartoon.sl
**
**      Cartoon shader
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
		float g_diffuseFactor;
		float g_diffuseEnvAngle;
		color g_envDiffuseColor;
		string diffuseEnvMap;
		string diffuseSampler;
		float bRenderDiffuse;
		float bRenderSpecular;
		output varying color outRefl;
	)
{
	
	vector worldEyeDir = I;
	vector worldNormal = Nf;

	color diff = Tex2DCombine(diffuseSampler, g_diffuse*g_envDiffuseColor*g_diffuseFactor, ss, tt);
	if ( diffuseEnvMap != "" )
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
	}

	return diff*bRenderDiffuse;
}

//--------------------------------------------------------------------
// ToonShader()
//--------------------------------------------------------------------
color ToonShader( float ss; float tt; color lightColor;
				  vector worldEyeDir; vector worldNormal; vector lightDir;
				  string diffuseSampler; float g_hasSpecular; color g_specular;
				  float g_specularPower; float g_colorTransition1; float g_colorTransition2;
				  string specularSampler; string glossSampler; float g_smoothness;
				  color g_midtone; color g_ambient; color g_diffuse;
				  float bEnableDiffuse; float bEnableSpecular)
{
	color rgb = 0;

	color diffColor = color(lightColor) * bEnableDiffuse;
	if( diffuseSampler != "" )
	{
		diffColor *= tex2D( diffuseSampler, ss, tt );
	}

	float dif = saturate(lightDir.worldNormal);

	float spec = 0;
	if( g_hasSpecular == 1 )
	{
		float spec = saturate( worldNormal.normalize(lightDir - worldEyeDir) );
		color specClr = g_specular * g_specularPower;
		float specTrans = g_colorTransition2;
		if( specularSampler != "" )
		{
			specClr = tex2D( specularSampler, ss, tt );
		}
		if( glossSampler != "" )
		{
			color specTransColor = tex2D( glossSampler, ss, tt );
			specTrans = specTransColor[0];
		}
		float interp = saturate( 1 - ((specTrans - spec) / g_smoothness) );
		rgb = specClr * lightColor * interp * bEnableSpecular;
	}

	float trans0 = g_colorTransition1 * 0.25;

	if( dif > g_colorTransition1 )
	{
		float interp = saturate(((dif - g_colorTransition1) / ((1 - g_colorTransition1) * g_smoothness )));
		diffColor *= lerp( g_midtone, g_diffuse, interp );
	}
	else if( dif > trans0 )
	{
		float interp = saturate(((dif - trans0) / ((g_colorTransition1 - trans0) * g_smoothness)));
		diffColor *= lerp( g_ambient, g_midtone, interp );
	}
	else
	{
		diffColor *= g_ambient;
	}

	rgb += diffColor;

	return rgb;
}

//--------------------------------------------------------------------
// Cartoon()
//--------------------------------------------------------------------
surface 
Cartoon( 
		string diffuseMap = "";
		string glossMap = "";
		string specularMap = "";
		string transparencyMap = "";
		color g_ambient = 0.5;
		color g_diffuse = 0.5; 
		color g_midtone = 0.5;
		color g_specular = 1; 
		float g_colorTransition1 = 0.5;
		float g_colorTransition2 = 1;
		float g_smoothness = 0.5;
		float g_specularPower = 1;
		float g_transparency = 1;
		float g_hasSpecular = 0;
		float g_diffuseFactor = 1;
		float g_diffuseEnvAngle = 0;
		color g_envDiffuseColor = 0.5;
		float g_specularFactor = 1; 
		float g_specularEnvAngle = 0;
		color g_envSpecularColor = 0;
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
				g_diffuse,
				g_diffuseFactor,
				g_diffuseEnvAngle,
				g_envDiffuseColor,
				diffuseEnvMap,
				diffuseMap,
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
		color outColor = ToonShader( ss, tt, Cl,
									 normalize(I), Nf, normalize(L),
									 diffuseMap, g_hasSpecular, g_specular,
									 g_specularPower, g_colorTransition1, g_colorTransition2,
									 specularMap, glossMap, g_smoothness+0.0001,
									 g_midtone, g_ambient, g_diffuse,
									 bEnableDiffuse, bEnableSpecular);

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
		
	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;
	Ci = Oi * Cs * C;
}







