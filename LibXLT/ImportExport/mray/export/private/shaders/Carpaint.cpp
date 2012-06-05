/******************************************************************************
 * Copyright 1986-2010 by mental images GmbH, Fasanenstr. 81, D-10623 Berlin,
 * Germany. All rights reserved.
 ******************************************************************************
 * Created:	20.10.97
 * Module:	baseshader
 * Purpose:	base shaders for Phenomenon writers
 *
 * Exports:
 *
 *      sgpu_illum_CarPaint()
 *
 * History:
 *      20.10.97: initial version
 *	17.11.97: added ambience parameter
 *	27.01.98: added global illumination capabilities
 *
 * Description:
 *      Perform illumination with the following reflection model:
 *      - Phong specular (cosine power) plus
 *      - Lambert diffuse (cosine) plus
 *      - ambient (constant)
 *****************************************************************************/

#ifdef HPUX
#pragma OPT_LEVEL 1	/* workaround for HP/UX optimizer bug, +O2 and +O3 */
#endif

#include <stdio.h>
#include <stdlib.h>		/* for abs */
#include <float.h>		/* for FLT_MAX */
#include <math.h>
#include <string.h>
#include <assert.h>
#include "shader.h"
#include "mi_shader_if.h"

#include "Support.h"


// must match the _decl.mi file for this shader!
struct sgpu_illum_CarPaint {

	// shader vars
	miTag		CubeReflMap;
	miTag		FlakeMap;
	miTag		iorMap;
	miTag		iorMap2;
	miTag		reflectFactorMap;
	miTag		transparencyMap;
	miTag		emissiveMap;
	miColor		flakeColor;
	miColor		g_diffuse;
	miColor		g_edgeColor;
	miColor		g_emissive;
	miColor		g_specular;
	miColor		g_specular2;
	miScalar	flakeTile;
	miScalar	flakeTolerance;
	miScalar	fresnelBias;
	miScalar	fresnelClrBias;
	miScalar	fresnelClrPower;
	miScalar	fresnelPower;
	miScalar	g_IOR;
	miScalar	g_IOR2;
	miScalar	g_emissiveIntensity;
	miScalar	g_reflectionEnvAngle;
	miScalar	g_reflectivity;
	miScalar	g_shininess;
	miScalar	g_shininess2;
	miScalar	g_transparency;
	miScalar	reflScale;
	miBoolean	g_useDynamicCubeMap;

	// env
	miTag		diffuseEnvMap;
	miTag		specularEnvMap;
	miColor		g_envDiffuseColor;
	miColor		g_envSpecularColor;
	miScalar	g_diffuseFactor;
	miScalar	g_diffuseEnvAngle;
	miScalar	g_specularFactor;
	miScalar	g_specularEnvAngle;

	// UV transform
    miScalar	u_scale;
    miScalar	v_scale;
    miScalar	u_offset;
    miScalar	v_offset;
	miScalar	uv_rotation;

	// Normal map
	miTag		normalMap;
	miScalar	normalMapScale;

	// AO params
	miBoolean	bEnableAO;	
	miColor		aoColor;
	miScalar	aoRadiusNear;
	miScalar	aoRadiusFar;
	miScalar	aoAngleBias;
	miScalar	aoAttenuation;
	miScalar	aoContrast;
	miScalar	aoCamNear;
	miScalar	aoCamFar;
	miScalar	aoSamples;

	// GI params
	miBoolean	bEnableGI;

	// SWL params
	miBoolean	bEnableSWL;

	int		mode;       /* light mode: 0..2 */
	int		i_light;	/* index of first light */
	int		n_light;	/* number of lights */
	miTag	light[1];	/* list of lights */
};

void EvalParams(miState *state,
				const sgpu_illum_CarPaint* paras,
				sgpu_illum_CarPaint& o_Params)
{ 
	// shader params
	o_Params.CubeReflMap = *mi_eval_tag(&paras->CubeReflMap);
	o_Params.FlakeMap = *mi_eval_tag(&paras->FlakeMap);
	o_Params.iorMap = *mi_eval_tag(&paras->iorMap);
	o_Params.iorMap2 = *mi_eval_tag(&paras->iorMap2);
	o_Params.reflectFactorMap = *mi_eval_tag(&paras->reflectFactorMap);
	o_Params.transparencyMap = *mi_eval_tag(&paras->transparencyMap);
	o_Params.emissiveMap = *mi_eval_tag(&paras->emissiveMap);
	o_Params.flakeColor = *mi_eval_color(&paras->flakeColor);
	o_Params.g_diffuse = *mi_eval_color(&paras->g_diffuse);
	o_Params.g_edgeColor = *mi_eval_color(&paras->g_edgeColor);
	o_Params.g_emissive = *mi_eval_color(&paras->g_emissive);
	o_Params.g_specular = *mi_eval_color(&paras->g_specular);
	o_Params.g_specular2 = *mi_eval_color(&paras->g_specular2);
	o_Params.flakeTile  = *mi_eval_scalar(&paras->flakeTile);
	o_Params.flakeTolerance  = *mi_eval_scalar(&paras->flakeTolerance);
	o_Params.fresnelBias  = *mi_eval_scalar(&paras->fresnelBias);
	o_Params.fresnelClrBias  = *mi_eval_scalar(&paras->fresnelClrBias);
	o_Params.fresnelClrPower  = *mi_eval_scalar(&paras->fresnelClrPower);
	o_Params.fresnelPower  = *mi_eval_scalar(&paras->fresnelPower);
	o_Params.g_IOR  = *mi_eval_scalar(&paras->g_IOR);
	o_Params.g_IOR2  = *mi_eval_scalar(&paras->g_IOR2);
	o_Params.g_emissiveIntensity  = *mi_eval_scalar(&paras->g_emissiveIntensity);
	o_Params.g_reflectionEnvAngle  = *mi_eval_scalar(&paras->g_reflectionEnvAngle);
	o_Params.g_reflectivity  = *mi_eval_scalar(&paras->g_reflectivity);
	o_Params.g_shininess  = *mi_eval_scalar(&paras->g_shininess);
	o_Params.g_shininess2  = *mi_eval_scalar(&paras->g_shininess2);
	o_Params.g_transparency  = *mi_eval_scalar(&paras->g_transparency);
	o_Params.reflScale  = *mi_eval_scalar(&paras->reflScale);
	o_Params.g_useDynamicCubeMap  = *mi_eval_boolean(&paras->g_useDynamicCubeMap);

	// env
	o_Params.diffuseEnvMap = *mi_eval_tag(&paras->diffuseEnvMap);
	o_Params.specularEnvMap = *mi_eval_tag(&paras->specularEnvMap);
	o_Params.g_envDiffuseColor = *mi_eval_color(&paras->g_envDiffuseColor);
	o_Params.g_envSpecularColor = *mi_eval_color(&paras->g_envSpecularColor);
	o_Params.g_diffuseFactor = *mi_eval_scalar(&paras->g_diffuseFactor);
	o_Params.g_diffuseEnvAngle = *mi_eval_scalar(&paras->g_diffuseEnvAngle);
	o_Params.g_specularFactor = *mi_eval_scalar(&paras->g_specularFactor);
	o_Params.g_specularEnvAngle = *mi_eval_scalar(&paras->g_specularEnvAngle);

	// UV transform
    o_Params.u_scale  = *mi_eval_scalar(&paras->u_scale);
    o_Params.v_scale  = *mi_eval_scalar(&paras->v_scale);
    o_Params.u_offset = *mi_eval_scalar(&paras->u_offset);
    o_Params.v_offset = *mi_eval_scalar(&paras->v_offset);
	o_Params.uv_rotation = *mi_eval_scalar(&paras->uv_rotation);

	// Normal map
	o_Params.normalMap = *mi_eval_tag(&paras->normalMap);
	o_Params.normalMapScale = *mi_eval_scalar(&paras->normalMapScale);

	// AO params
	o_Params.bEnableAO = *mi_eval_boolean(&paras->bEnableAO);
	o_Params.aoColor = *mi_eval_color(&paras->aoColor);
	o_Params.aoRadiusNear = *mi_eval_scalar(&paras->aoRadiusNear);
	o_Params.aoRadiusFar = *mi_eval_scalar(&paras->aoRadiusFar);
	o_Params.aoAngleBias = *mi_eval_scalar(&paras->aoAngleBias);
	o_Params.aoAttenuation = *mi_eval_scalar(&paras->aoAttenuation);
	o_Params.aoContrast = *mi_eval_scalar(&paras->aoContrast);
	o_Params.aoCamNear = *mi_eval_scalar(&paras->aoCamNear);
	o_Params.aoCamFar = *mi_eval_scalar(&paras->aoCamFar);
	o_Params.aoSamples = *mi_eval_scalar(&paras->aoSamples);

	// GI params
	o_Params.bEnableGI = *mi_eval_boolean(&paras->bEnableGI);	
	
	// SWL params
	o_Params.bEnableSWL = *mi_eval_boolean(&paras->bEnableSWL);

}

miColor iblPS(miState * state,
			  miVector sstt,
			  miVector worldNormal,
			  miVector worldEyeDir,
			  miTag	diffuseEnvMap,
			  miColor g_envDiffuseColor,			  
			  miScalar g_diffuseFactor,
			  miScalar g_diffuseEnvAngle,
			  miTag	specularEnvMap,
			  miColor g_envSpecularColor,
			  miScalar g_specularFactor,
			  miScalar g_specularEnvAngle,
			  miColor g_diffuse,
			  miScalar g_reflectivity,
			  miColor g_emissive,
			  miTag emissiveMap,
			  miScalar g_emissiveIntensity,
			  miTag CubeReflMap,
			  miScalar reflScale,
			  miScalar fresnelBias,
			  miScalar fresnelPower,
			  miScalar g_reflectionEnvAngle,
			  miTag reflectFactorMap,
			  bool bRenderSpecular,
			  bool bRenderDiffuse,
			  miBoolean bEnableSWL
			  ) 
{
	miColor outColor = make_color(0);

	miColor refl = make_color(0);
	if (CubeReflMap)
	{
		miVector incid;
		miVector norm;
		mi_vector_to_world(state,&incid,&worldEyeDir);
		mi_vector_to_world(state,&norm,&worldNormal);
		incid = normalize(incid);
		norm = normalize(norm);
		float rmanBlur = 0.002222f*reflScale*reflScale - 0.005f*reflScale;
		float ndv = dot((incid),norm);
		float fresnel = saturate(fastFresnel( ndv, fresnelBias, fresnelPower ));
		refl = SampleEnvironmentLOD(state, (worldEyeDir*-1), worldNormal, CubeReflMap, g_reflectionEnvAngle, rmanBlur);
		refl *= g_reflectivity;
		refl *= fresnel;
		refl = Tex2dCombine(state, reflectFactorMap, refl, sstt);
	}
	PutRefl(&refl, state, true);

	miColor spec = make_color(0), diff = make_color(0);
	if (!bEnableSWL)
	{
		spec = g_envSpecularColor*g_specularFactor;
		if ( specularEnvMap )
		{
			spec *= SampleEnvironment(state, (worldEyeDir*-1), worldNormal, specularEnvMap, g_specularEnvAngle);
		}

		diff = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
		if ( diffuseEnvMap )
		{
			diff *= SampleEnvDiffuse(state, worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
		}	
	}
	else
	{
		//miColor env = GetEnvironment(state);

		////spec = g_reflectivity*g_specularFactor*make_color(0);
		//diff = g_diffuse*env;
	}
	
	miColor emissive = Tex2dCombine(state, emissiveMap, g_emissive*g_emissiveIntensity, sstt);
	outColor = emissive + spec*g_reflectivity*bRenderSpecular + refl + diff*bRenderDiffuse;

	return outColor;
}

//--------------------------------------------------------------------
// PaintShaderSpecular()
//--------------------------------------------------------------------
miColor PaintShaderSpecular(miState * state, miVector sstt, miVector worldNormal, miVector lightDir, 
						    miVector worldEyeDir, float SpecPowerScale,
						    miTag iorMap, float g_IOR, miColor g_specular,
						    miTag iorMap2, float g_IOR2, miColor g_specular2,
						    float g_shininess2, miColor lightColorSpec) 
{

	miColor ior_color = Tex2dCombine( state, iorMap, make_color(g_IOR-1,g_IOR-1,g_IOR-1, 1), sstt );
	float ior = ior_color.r + 1;

	float g_specularBias = 0;
	float g_specularBias2 = 0;

	miColor specular = BlinnSpecular(state, worldNormal, lightDir, worldEyeDir*-1, 
		               lightColorSpec, g_specular, SpecPowerScale, ior, g_specularBias, 1);

	miColor ior2_color = Tex2dCombine( state, iorMap2, make_color(g_IOR2-1,g_IOR2-1,g_IOR2-1,1), sstt );
	float ior2 = ior2_color.r + 1;

	miColor specular2 = BlinnSpecular(state, worldNormal, lightDir, worldEyeDir*-1, 
				       lightColorSpec, g_specular2, g_shininess2, ior2, g_specularBias2, 1);

	return specular + specular2;
}

//--------------------------------------------------------------------
// doFlakes()
//--------------------------------------------------------------------
miColor doFlakes(miState * state, miVector sstt, 
				 miVector worldEyeDir, miVector lightDir, miTag flakeSampler, 
				 float flakeTile, miColor flakeColor, float flakeTolerance) 
{
	miVector flakeNormal = Tex2dNormal( state, flakeSampler, sstt * flakeTile );

	//float det = Du(ss)*Dv(tt) - Du(tt)*Dv(ss);
	//vector udir = normalize((Du(P)*Dv(tt) - Dv(P)*Du(tt))/det);
	//vector vdir = normalize((Dv(P)*Du(ss) - Du(P)*Dv(ss))/det);

	miVector basisx = normalize(state->bump_x_list[0]);
	miVector basisy = normalize(state->bump_y_list[0]);
	miVector basisz = normalize(state->normal);

	miMatrix tangentMat = { basisx.x, basisx.y, basisx.z, 0 ,
							basisy.x, basisy.y, basisy.z, 0 ,
							basisz.x, basisz.y, basisz.z, 0 ,
							0       , 0       , 0       , 1 };

	miVector worldFlakeNormal;
	mi_vector_transform( &worldFlakeNormal, &flakeNormal, tangentMat );

	//vector worldFlakeNormal = transform( tangentMat , flakeNormal );

	worldFlakeNormal = normalize( worldFlakeNormal );

	miVector sparkRefl = reflect( worldEyeDir, worldFlakeNormal );

	float val = dot(sparkRefl,lightDir);

	miColor rgb = val * flakeColor;
	if ( val <= flakeTolerance )
	{
		rgb = make_color(0);
	}	
	return rgb;
}

//--------------------------------------------------------------------
// PaintShaderPS()
//--------------------------------------------------------------------
miColor PaintShaderPS( miState * state, miVector sstt, 
					 float SpecPowerScale,
					 miVector incident, miVector ldir, miVector Nf,					 
					 miTag iorMap, float g_IOR, miColor g_specular,
					 miTag iorMap2, float g_IOR2, miColor g_specular2,
					 float g_shininess2, float fresnelClrBias, float fresnelClrPower,
					 miColor g_diffuse, miColor g_edgeColor,
					 miTag FlakeMap, float flakeTile, miColor flakeColor, float flakeTolerance,
					 float bEnableDiffuse, float bEnableSpecular,
					 miColor lightColorDiff, miColor lightColorSpec)
{
    miColor rgb;

	miVector lightDir = normalize(ldir);
	miVector worldEyeDir = normalize(incident);
	miVector worldNormal = normalize(Nf);

	rgb = PaintShaderSpecular( state, sstt, worldNormal, lightDir, worldEyeDir*-1, SpecPowerScale,
							   iorMap, g_IOR, g_specular, iorMap2, g_IOR2, g_specular2, g_shininess2, lightColorSpec) * (bEnableSpecular?1.0f:0.0f);

	//calculate car paint color	
	float ndv = dot((worldEyeDir),worldNormal);
	float Cfresnel = saturate(fastFresnel( ndv, fresnelClrBias, fresnelClrPower ));

	miColor Paint = lerp( g_diffuse, g_edgeColor, Cfresnel );


	miColor diffuse = PhongDiffuse(worldNormal, lightDir, lightColorDiff, Paint) * (bEnableDiffuse?1.0f:0.0f);

	rgb += diffuse;
	
	//calculate Speckle Color
	//fetch flake normal
	if( FlakeMap )
	{
		rgb += doFlakes(state, sstt, worldEyeDir,lightDir, FlakeMap, flakeTile, flakeColor, flakeTolerance);
	}

	return rgb;
}


extern "C" DLLEXPORT int sgpu_illum_CarPaint_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_illum_CarPaint(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_CarPaint *paras)
{

	miTag		*light;		/* tag of light instance */
	int			n_l;		/* number of light sources */
	int			i_l;		/* offset of light sources */
	int			m;			/* light mode: 0=all, 1=incl, 2=excl */
	int			samples;	/* # of samples taken */
	miColor		sum;		/* summed sample colors */
	int size;

    /* check for illegal calls */
    if (state->type == miRAY_SHADOW || state->type == miRAY_DISPLACE ) {
		return(miFALSE);
	}

	struct sgpu_illum_CarPaint o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);
	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

	float g_transparency = o_Params.g_transparency * Tex2d(state, o_Params.transparencyMap, sstt).r;

	miColor sDiffColor = o_Params.g_diffuse;

	bool bRenderSpecular = true;
	bool bRenderDiffuse = true;

	// IBL_PS
	*result = iblPS( state,
					 sstt,
					 worldNormal,
					 worldEyeDir,
					 o_Params.diffuseEnvMap,
					 o_Params.g_envDiffuseColor,
					 o_Params.g_diffuseFactor,
					 o_Params.g_diffuseEnvAngle,
					 o_Params.specularEnvMap,
					 o_Params.g_envSpecularColor,
					 o_Params.g_specularFactor,
					 o_Params.g_specularEnvAngle,
					 o_Params.g_diffuse,
					 o_Params.g_reflectivity,
					 o_Params.g_emissive,
					 o_Params.emissiveMap,
					 o_Params.g_emissiveIntensity,					 
				     o_Params.CubeReflMap,
				     o_Params.reflScale,
				     o_Params.fresnelBias,
				     o_Params.fresnelPower,
				     o_Params.g_reflectionEnvAngle,
				     o_Params.reflectFactorMap,
				     bRenderSpecular,
				     bRenderDiffuse,
					 o_Params.bEnableSWL);

	// lighting loop
	m     = *mi_eval_integer(&paras->mode);
	n_l   = *mi_eval_integer(&paras->n_light);
	i_l   = *mi_eval_integer(&paras->i_light);
	light =  mi_eval_tag(paras->light) + i_l;
	if (m == 1)		/* modify light list (inclusive mode) */
		mi_inclusive_lightlist(&n_l, &light, state);
	else if (m == 2)	/* modify light list (exclusive mode) */
		mi_exclusive_lightlist(&n_l, &light, state);
	else if (m == 4) {
		n_l = 0;
		light = 0;
	}

	/* Loop over all light sources */
	if (m == 4 || n_l) {

		for (mi::shader::LightIterator iter(state, light, n_l);
						!iter.at_end(); ++iter) {
			sum = make_color(0);

			while (iter->sample()) {

				// Light info
				miColor	lightColor;
				iter->get_contribution(&lightColor);
				miVector lightDir = iter->get_direction();
				mi_vector_to_world(state,&lightDir,&lightDir);
				lightDir = normalize(lightDir);

				sgpuLightData* info = (sgpuLightData*)mi_shaderstate_get( state, SGPU_LIGHT_DATA_NAME, &size );
				bool bMSPLight = (info) && !(o_Params.bEnableSWL && iter->get_current() == iter->get_number_of_lights()-1);
				bool AffectsDiffuse = (bMSPLight) ? info->AffectsDiffuse : true;
				bool AffectsSpecular = (bMSPLight) ? info->AffectsSpecular : true;

				if (!bMSPLight)
					lightColor = GetScaledIBL(lightColor);

				// Calculate color
				miColor outColor = PaintShaderPS( state, sstt, o_Params.g_shininess,
					worldEyeDir, normalize(lightDir), worldNormal,
					o_Params.iorMap, o_Params.g_IOR, o_Params.g_specular,
					o_Params.iorMap2, o_Params.g_IOR2, o_Params.g_specular2,
					o_Params.g_shininess2, o_Params.fresnelClrBias, o_Params.fresnelClrPower,
					o_Params.g_diffuse, o_Params.g_edgeColor,
					o_Params.FlakeMap, o_Params.flakeTile, o_Params.flakeColor, o_Params.flakeTolerance,
					AffectsDiffuse, AffectsSpecular,
					lightColor, lightColor);

				sum += outColor;
				
			}

			samples = iter->get_number_of_samples();
			if (samples) {
				*result += sum / (miScalar)samples;
			}
		}
	}

	// AO
	miColor aoColor;
	GetAO(&aoColor,state,o_Params.bEnableAO,o_Params.aoColor,o_Params.aoRadiusNear,o_Params.aoRadiusFar,o_Params.aoAngleBias,
		  o_Params.aoAttenuation,o_Params.aoContrast,o_Params.aoCamNear,o_Params.aoCamFar,o_Params.aoSamples);
	*result *= aoColor;

	// FG
	miColor giColor;
	GetGI(&giColor,state,o_Params.bEnableGI);
	*result += giColor * sDiffColor;

	// transparency blending
	if (g_transparency < 1)
	{
		miColor trans;
		mi_trace_transparent(&trans, state);
		*result = (*result)*g_transparency + trans*(1.0f - g_transparency);
	}

	return(miTRUE);
}


