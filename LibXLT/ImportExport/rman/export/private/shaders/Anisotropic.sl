/*****************************************************************************
**  Anisotropic.sl
**
**      Anisotropic shader
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
		float g_diffuseFactor;
		float g_diffuseEnvAngle;
		color g_envDiffuseColor;
		float g_specularFactor;
		float g_specularEnvAngle;
		color g_envSpecularColor;
		string reflectFactorMap; 
		string diffuseEnvMap;
		string specularEnvMap;
		string CubeReflMap;
		float g_reflectionEnvAngle;
		float g_emissiveIntensity;
		float bRenderDiffuse;
		float bRenderSpecular;
		float reflScale;
		output varying color outRefl;
	)
{
	color outColor = 0;
	
	vector worldEyeDir = I;
	vector worldNormal = Nf;

	color spec = g_envSpecularColor*g_specularFactor;
	if ( specularEnvMap != "" )
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, specularEnvMap, g_specularEnvAngle);
	}
	
	color diff = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
	if ( diffuseEnvMap != "" )
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
	}

	color refl = 0;
	if (CubeReflMap != "")
	{
		vector incid = normalize(transform("world",I));
		vector norm = normalize(transform("world",worldNormal));
		float rmanBlur = 0.002222*reflScale*reflScale - 0.005*reflScale;
		float ndv = (-incid).norm;
		float fresnel = saturate(fastFresnel( ndv, fresnelBias, fresnelPower ));
		refl = SampleEnvironmentLOD(-worldEyeDir, worldNormal, CubeReflMap, g_reflectionEnvAngle, rmanBlur);
		refl *= g_reflectivity;
		refl *= fresnel;
		refl = Tex2DCombine(reflectFactorMap, refl,ss,tt);
	}

	outRefl = refl;
	
	outColor = color( g_emissive*g_emissiveIntensity + spec*g_reflectivity*bRenderSpecular + refl + diff*bRenderDiffuse );

	return outColor;
}

//--------------------------------------------------------------------
// WardAnisotropicSpecular()
//--------------------------------------------------------------------
float WardAnisotropicSpecular(vector norm; vector lightDir; vector eyeDir; vector Tangent; vector Binormal; float i_Ax; float i_Ay )
{
	vector half = normalize(eyeDir + lightDir);
	
	// Set Ax, Ay make sure wx, wy they are not going to zero
	float Ay = clamp(i_Ay, 0.1, 89.9) * (PI/180);
	float Ax = clamp(i_Ax, 0, 1.0);	

	float wx = 0.5 - 0.5 * Ax * (cos(Ay) - sin(Ay));
	float wy = 0.5 + 0.5 * Ax * (cos(Ay) - sin(Ay));

	//precalc
	float NdotL = norm.lightDir;
	float NdotH = norm.half;
	float NdotV = norm.eyeDir;
	float HdotX = half.Tangent / wx;
	float HdotY = half.Binormal / wy;

	//specular distribution term
	float power = -2 * ( (HdotX*HdotX + HdotY*HdotY) / (1 + NdotH) );
	float coeff = 1/sqrt( NdotL * NdotV );

	float reflect = NdotL / (4 * wx * wy);

	float term = coeff * reflect * exp( power );

	if( NdotL < 0 || NdotV < 0 )
	{
		term = 0;
	}

	return term;
}

//--------------------------------------------------------------------
// AnisotropicShaderSpecular()
//--------------------------------------------------------------------
color AnisotropicShaderSpecular( float ss; float tt; color lightColor;
								 vector worldNormal; vector lightDir; vector worldEyeDir; 
								 string specularSampler; color g_specular;
								 string specularColorPowerSampler; float SpecularColorPower;
								 string shininessSampler; float SpecularPowerScale; 
								 string anisotropySampler; float g_UPower; float g_VPower)
{

	float det = Du(ss)*Dv(tt) - Du(tt)*Dv(ss);
	vector worldBinorm = normalize((Dv(P)*Du(ss) - Du(P)*Dv(ss))/det);
	vector worldTan = -normalize((Du(P)*Dv(tt) - Dv(P)*Du(tt))/det);

	color specularCombine = Tex2DCombine(specularSampler, g_specular, ss, tt);

	color specularPowerCombineColor = Tex2DCombine(specularColorPowerSampler, 
												   color(SpecularColorPower,SpecularColorPower,SpecularColorPower), 
												   ss, tt);
	float specularPowerCombine = specularPowerCombineColor[0];
	
	color shininessCombineColor = Tex2DCombine(shininessSampler, 
											   color(SpecularPowerScale,SpecularPowerScale,SpecularPowerScale),
											   ss, tt);
	float shininessCombine = shininessCombineColor[0];

	float anisotropy = WardAnisotropicSpecular( worldNormal, lightDir, -worldEyeDir, worldTan, worldBinorm, g_UPower, g_VPower );

	color anisotropyCombineColor = Tex2DCombine(anisotropySampler,
												color(anisotropy,anisotropy,anisotropy),
												ss, tt);
	float anisotropyCombine = saturate( anisotropyCombineColor[0] );

	color specular = specularPowerCombine * lightColor * specularCombine * pow(anisotropyCombine, max(shininessCombine, 0.001)) ;
	
	return specular;
}

//--------------------------------------------------------------------
// AnisotropicShaderPS()
//--------------------------------------------------------------------
color AnisotropicShaderPS(float ss; float tt; color lightColor;
						  vector worldNormal; vector lightDir; vector worldEyeDir; 
						  string specularSampler; color g_specular;
						  string specularColorPowerSampler; float SpecularColorPower;
						  string shininessSampler; float SpecularPowerScale; 
						  string anisotropySampler; float g_UPower; float g_VPower;
						  string diffuseSampler; color g_diffuse;
						  float bEnableDiffuse; float bEnableSpecular)
{
	color rgb = AnisotropicShaderSpecular( ss, tt, lightColor,
										   worldNormal, lightDir, worldEyeDir, 
										   specularSampler, g_specular,
										   specularColorPowerSampler, SpecularColorPower,
										   shininessSampler, SpecularPowerScale, 
										   anisotropySampler, g_UPower, g_VPower) * bEnableSpecular;


	color diff = Tex2DCombine(diffuseSampler, g_diffuse, ss, tt);

	color diffuse = PhongDiffuse(worldNormal, lightDir, lightColor, diff);
	
	rgb += diffuse * bEnableDiffuse;

	return rgb;
}

//--------------------------------------------------------------------
// Anisotropic()
//--------------------------------------------------------------------
surface 
Anisotropic( 
		string CubeReflMap = "";
		string anisotropyMap = "";
		string diffuseMap = "";
		string reflectFactorMap = "";
		string shininessMap = "";
		string specularColorPowerMap = "";
		string specularMap = "";
		string transparencyMap = "";
		color g_diffuse = 0.5; 
		color g_emissive = 0;
		color g_specular = 0.5;
		float SpecularColorPower = 1;
		float fresnelBias = 0.2; 
		float fresnelPower = 4;
		float g_UPower = 1;
		float g_VPower = 6.3;
		float g_emissiveIntensity = 1;
		float g_reflectionEnvAngle = 0;
		float g_reflectivity = 1;
		float g_shininess = 0.2;
		float g_transparency = 1;
		float reflScale = 0;
		float g_diffuseFactor = 1;
		float g_diffuseEnvAngle = 0;
		color g_envDiffuseColor = 0.5;
		float g_specularFactor = 1; 
		float g_specularEnvAngle = 0;
		color g_envSpecularColor = 0;
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
		float camFarClip = 1;
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
				g_emissive,
				g_specular,
				fresnelBias,
				fresnelPower,
				g_reflectivity,
				g_diffuseFactor,
				g_diffuseEnvAngle,
				g_envDiffuseColor,
				g_specularFactor,
				g_specularEnvAngle,
				g_envSpecularColor,
				reflectFactorMap,
				diffuseEnvMap,
				specularEnvMap,
				CubeReflMap,
				g_reflectionEnvAngle,
				g_emissiveIntensity,
				bRenderDiffuse,
				bRenderSpecular,
				reflScale,
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
		color outColor = AnisotropicShaderPS(ss, tt, Cld,
											 Nf, normalize(L), normalize(I), 
											 specularMap, g_specular,
											 specularColorPowerMap, SpecularColorPower,
											 shininessMap, g_shininess, 
											 anisotropyMap, g_UPower, g_VPower,
											 diffuseMap, g_diffuse,
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






