/*****************************************************************************
**  SpecularFresnel.sl
**
**      SpecularFresnel shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\***************************************s*************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// IBL()
//--------------------------------------------------------------------
color IBL(
	    normal Nf;
		float ss;
		float tt;
		color g_diffuse;
		float g_reflectivity;
		float g_diffuseFactor;
		float g_diffuseEnvAngle;
		color g_envDiffuseColor;
		float g_specularFactor;
		float g_specularEnvAngle;
		color g_envSpecularColor;
		string diffuseMap;
		string diffuseEnvMap;
		string specularEnvMap;
		float bRenderDiffuse;
		float bRenderSpecular;
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
	
	color diff = Tex2DCombine(diffuseMap, g_diffuse*g_envDiffuseColor*g_diffuseFactor, ss, tt);
	if ( diffuseEnvMap != "" )
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
	}
	
	outColor = color( spec*g_reflectivity*bRenderSpecular + diff*bRenderDiffuse );

	return outColor;
}

//--------------------------------------------------------------------
// ShiftTangent()
//--------------------------------------------------------------------
vector ShiftTangent(vector T; vector norm; float shift)
{
	vector shiftedT = T + shift*norm;
	return normalize(shiftedT);
}

//--------------------------------------------------------------------
// StrandSpecular()
//--------------------------------------------------------------------
float StrandSpecular(vector T; vector H; float exponent)
{
	float dotTH = T.H;
	float sinTH = saturate(sqrt(1.0 - dotTH*dotTH));
	float dirAtten = smoothstep(-1.0, 0.0, dotTH);
	float retVal = 0;
	if ( sinTH != 0 )
	{
		retVal = dirAtten * pow(sinTH, exponent);
	}
	return retVal;
}

//--------------------------------------------------------------------
// HairDiffuseContrib()
//--------------------------------------------------------------------
color HairDiffuseContrib(vector norm; vector lightVec; color lightColor; color g_hairBaseColor)
{
	color diffuse = saturate( lerp(0.25,1.0, norm.lightVec) );
	diffuse *= lightColor * g_hairBaseColor;	
	return diffuse;
}

//--------------------------------------------------------------------
// HairSpecularContrib()
//--------------------------------------------------------------------
color HairSpecularContrib(vector tangent; vector norm; vector halfVec;	float ss; float tt; 
						  string tSpecularShift; float g_specularShift0; float g_specularShift1;
						  string tSpecularMask; float g_specularExp0; float g_specularExp1;
						  color g_specularColor0; color g_specularColor1)
{
	// shift tangents	
	color shiftTexColor = Tex2DReplace(tSpecularShift, color(0.5,0.5,0.5), ss , tt );
	float shiftTex = shiftTexColor[0] - 0.5;
	
	vector t1 = ShiftTangent(tangent, norm, g_specularShift0 + shiftTex);
	vector t2 = ShiftTangent(tangent, norm, g_specularShift1 + shiftTex);

	// 2 hilights of different colors, specular exponents, and differently shifted tangents.
	// add 2nd specular term, modulated with noise texture
	color specMaskColor = Tex2DReplace(tSpecularMask, color(1,1,1), ss, tt);
	float specMask = specMaskColor[0]; // approximate sparkles using texture

	// specular lighting
	color specular = (g_specularColor0 * StrandSpecular(t1, halfVec, g_specularExp0) + 
					  g_specularColor1 * StrandSpecular(t2, halfVec, g_specularExp1)) * specMask;
	
	return specular;
}

//--------------------------------------------------------------------
// HairLighting()
//--------------------------------------------------------------------
color HairLighting(vector tangent; vector norm; vector lightVec;
				   vector halfVec; float ss; float tt; float ambOcc; 
				   color lightDiffColor; color lightSpecColor; color g_hairBaseColor; 
				   string tSpecularShift; float g_specularShift0; float g_specularShift1;
				   string tSpecularMask; float g_specularExp0; float g_specularExp1;
				   color g_specularColor0; color g_specularColor1;
				   string tBaseSampler; float bEnableDiffuse; float bEnableSpecular)
{
	color diffuse = HairDiffuseContrib(norm, lightVec, lightDiffColor, g_hairBaseColor) * bEnableDiffuse;

	
	color specular = HairSpecularContrib(tangent, norm, halfVec, ss, tt,
										 tSpecularShift, g_specularShift0, g_specularShift1,
										 string tSpecularMask, g_specularExp0, g_specularExp1,
										 g_specularColor0, g_specularColor1) * bEnableSpecular;

    // specular attenuation for hair facing away from light
    float specularAttenuation = saturate(1.75 * (norm.lightVec) + 0.5);
	
	color base = Tex2DReplace(tBaseSampler, color(1,1,1), ss, tt);

	// final color assembly
	color rgb = diffuse * base;
    rgb += (specular * base * specularAttenuation * lightSpecColor);	

	return rgb;
}

//--------------------------------------------------------------------
// SpecularFresnel()
//--------------------------------------------------------------------
surface 
SpecularFresnel( 	
		string tAlpha = "";
		string tBase = "";
		string tSpecularMask = ""; 
		string tSpecularShift = "";
		color g_hairBaseColor = 0.5;
		color g_specularColor0 = 0.5; 
		color g_specularColor1 = 0.5;
		float g_reflectivity = 1;
		float g_specularExp0 = 100;
		float g_specularExp1 = 100;
		float g_specularShift0 = -0.1500000059604645;
		float g_specularShift1 = 0.1500000059604645;
		float g_transparency = 1;
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
		string tNormalMap = "";
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
		color g_ambient = 1;
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
				g_hairBaseColor,
				g_reflectivity,
				g_diffuseFactor,
				g_diffuseEnvAngle,
				g_envDiffuseColor,
				g_specularFactor,
				g_specularEnvAngle,
				g_envSpecularColor,
				tBase,
				diffuseEnvMap,
				specularEnvMap,
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
		// the (0,1,0) direction in tangent space:

		// DETERMINANT OF THE JACOBIAN
		float det = Du(ss)*Dv(tt) - Du(tt)*Dv(ss);
		vector worldTan = normalize((Dv(P)*Du(ss) - Du(P)*Dv(ss))/det); // dPdt
		//vector worldTan = normalize((Du(P)*Dv(tt) - Dv(P)*Du(tt))/det); // dPds

		vector halfVec = normalize(-I) + normalize(L);

		color outColor = HairLighting(worldTan, Nf, normalize(L), normalize(halfVec), 
									  ss, tt, 1.0,
									  Cld, Cls, g_hairBaseColor,
									  tSpecularShift, g_specularShift0, g_specularShift1,
									  tSpecularMask, g_specularExp0, g_specularExp1,
									  g_specularColor0, g_specularColor1, 
									  tBase, bEnableDiffuse, bEnableSpecular) * bEnableLight;
									  		
		C += outColor * bEnableLight * bRenderLit;
	}
	
	CalculateGIAO(C, P, Nf, camNearClip, camFarClip, 
				  GISamples, GIMaxVariation, GIMaxDist, GIConeAngle, bRenderGI,
				  GIColor, GIRadiusNear, GIRadiusFar, GIAngleBias, GIAttenuation, GIContrast,
				  AOSamples, AOMaxVariation, AOMaxDist, AOConeAngle, bRenderAO,
				  AOColor, AORadiusNear, AORadiusFar, AOAngleBias, AOAttenuation, AOContrast, 
				  outGI 
				  );

	color alphaColor = Tex2DReplace(tAlpha, color(0,0,0), ss, tt);
	float alpha = alphaColor[0];
	color invAlpha = color(1,1,1) - alpha;
	invAlpha *= g_transparency;
	
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;

	Oi = invAlpha * doTransparency * objVisible;
	Ci = Oi * Cs * C;
}







