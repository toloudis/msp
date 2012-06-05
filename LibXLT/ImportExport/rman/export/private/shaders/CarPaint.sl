/*****************************************************************************
**  CarPaint.sl
**
**      CarPaint shader
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
		float g_useDynamicCubeMap;
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
// PaintShaderSpecular()
//--------------------------------------------------------------------
color PaintShaderSpecular(float ss; float tt; vector worldNormal; vector lightDir; 
						  vector worldEyeDir; float SpecPowerScale;
						  string iorMap; float g_IOR; color g_specular;
						  string iorMap2; float g_IOR2; color g_specular2;
						  float g_shininess2; color lightColorSpec) {

	color ior_color = Tex2DCombine( iorMap, color(g_IOR-1,g_IOR-1,g_IOR-1), ss, tt );
	float ior = ior_color[0] + 1;

	float g_specularBias = 0;
	float g_specularBias2 = 0;

	color specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		              lightColorSpec, g_specular, SpecPowerScale, ior, g_specularBias, 1);

	color ior2_color = Tex2DCombine( iorMap2, color(g_IOR2-1,g_IOR2-1,g_IOR2-1), ss, tt );
	float ior2 = ior2_color[0] + 1;

	color specular2 = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
				       lightColorSpec, g_specular2, g_shininess2, ior2, g_specularBias2, 1);

	return specular + specular2;
}

//--------------------------------------------------------------------
// doFlakes()
//--------------------------------------------------------------------
color doFlakes(vector worldEyeDir; vector lightDir;
				string flakeSampler; float ss; float tt;
				float flakeTile; color flakeColor; float flakeTolerance) {

	vector flakeNormal = Tex2DNormal( flakeSampler, ss * flakeTile, tt * flakeTile );

	float det = Du(ss)*Dv(tt) - Du(tt)*Dv(ss);
	vector udir = normalize((Du(P)*Dv(tt) - Dv(P)*Du(tt))/det);
	vector vdir = normalize((Dv(P)*Du(ss) - Du(P)*Dv(ss))/det);

	vector basisx = normalize(udir);
	vector basisy = normalize(vdir);
	vector basisz = normalize(N);

	matrix tangentMat = matrix( xcomp(basisx), ycomp(basisx), zcomp(basisx), 0 ,
								xcomp(basisy), ycomp(basisy), zcomp(basisy), 0 ,
								xcomp(basisz), ycomp(basisz), zcomp(basisz), 0 ,
								0            , 0            , 0            , 1 );

	vector worldFlakeNormal = transform( tangentMat , flakeNormal );

	worldFlakeNormal = normalize( worldFlakeNormal );


	vector sparkRefl = reflect( worldEyeDir, worldFlakeNormal );

	float val = sparkRefl.lightDir;

	color rgb = val * flakeColor;
	if ( val <= flakeTolerance )
	{
		rgb = 0;
	}
	
	return rgb;
}

//--------------------------------------------------------------------
// PaintShaderPS()
//--------------------------------------------------------------------
color PaintShaderPS( float SpecPowerScale;
					 vector incident; vector ldir; vector Nf;
					 float ss; float tt;
					 string iorMap; float g_IOR; color g_specular;
					 string iorMap2; float g_IOR2; color g_specular2;
					 float g_shininess2; float fresnelClrBias; float fresnelClrPower;
					 color g_diffuse; color g_edgeColor;
					 string FlakeMap; float flakeTile; color flakeColor; float flakeTolerance;
					 float bEnableDiffuse; float bEnableSpecular;
					 color lightColorDiff; color lightColorSpec)
{
    color rgb; 

	vector lightDir = normalize(ldir);
	vector worldEyeDir = normalize(incident);
	vector worldNormal = normalize(Nf);

	rgb = PaintShaderSpecular( ss, tt, worldNormal, lightDir, worldEyeDir, SpecPowerScale,
							   iorMap, g_IOR, g_specular, iorMap2, g_IOR2, g_specular2, g_shininess2, lightColorSpec) * bEnableSpecular;

	//calculate car paint color	
	float ndv = (-worldEyeDir).worldNormal;
	float Cfresnel = saturate(fastFresnel( ndv, fresnelClrBias, fresnelClrPower ));

	color Paint = lerp( g_diffuse, g_edgeColor, Cfresnel );
	
	color diffuse = PhongDiffuse(worldNormal, lightDir, lightColorDiff, Paint) * bEnableDiffuse;

	rgb += diffuse;
	
	//calculate Speckle Color
	//fetch flake normal
	if( FlakeMap != "" )
	{
		rgb += doFlakes(worldEyeDir,lightDir, FlakeMap, ss, tt, flakeTile, flakeColor, flakeTolerance);
	}

	return rgb;
}

//--------------------------------------------------------------------
// CarPaint()
//--------------------------------------------------------------------
surface 
CarPaint( 
		string CubeReflMap = "";
		string FlakeMap = "";
		string iorMap = "";
		string iorMap2 = "";
		string reflectFactorMap = "";
		string transparencyMap = "";
		color flakeColor = 0.1;
		color g_diffuse = 0.5;
		color g_edgeColor = 1;
		color g_emissive = 0;
		color g_specular = 1;
		color g_specular2 = 0;
		color g_ambient = 0;
		float flakeTile = 20;
		float flakeTolerance = 0;
		float fresnelBias = 0.2;
		float fresnelClrBias = 0.2;
		float fresnelClrPower = 4;
		float fresnelPower = 4;
		float g_IOR = 0.5;
		float g_IOR2 = 0.5;
		float g_reflectionEnvAngle = 0;
		float g_reflectivity = 1;
		float g_shininess = 1; 
		float g_shininess2 = 0.5;
		float g_transparency = 1;
		float reflScale = 0;
		float g_useDynamicCubeMap = 0;
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
		string NormalMap = "";
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
				dynamicReflection,
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
		color outColor = PaintShaderPS( g_shininess,
										I, normalize(L), Nf, ss, tt,
										iorMap, g_IOR, g_specular,
										iorMap2, g_IOR2, g_specular2,
										g_shininess2, fresnelClrBias, fresnelClrPower,
										g_diffuse, g_edgeColor,
										FlakeMap, flakeTile, flakeColor, flakeTolerance,
										bEnableDiffuse, bEnableSpecular,
										Cld, Cls);

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






