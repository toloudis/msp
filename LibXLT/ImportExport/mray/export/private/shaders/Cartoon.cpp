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
 *      sgpu_illum_Cartoon()
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
struct sgpu_illum_Cartoon {

	// shader vars
	miTag		diffuseMap;
	miTag		glossMap;
	miTag		specularMap;
	miTag		transparencyMap;
	miColor		g_ambient;
	miColor		g_diffuse;
	miColor		g_midtone;
	miColor		g_specular;
	miScalar	g_colorTransition1;
	miScalar	g_colorTransition2;
	miScalar	g_smoothness;
	miScalar	g_specularPower;
	miScalar	g_transparency;
	miBoolean	g_hasSpecular;

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
				const sgpu_illum_Cartoon* paras,
				sgpu_illum_Cartoon& o_Params)
{ 

	// shader params
	o_Params.diffuseMap = *mi_eval_tag(&paras->diffuseMap);
	o_Params.glossMap = *mi_eval_tag(&paras->glossMap);
	o_Params.specularMap = *mi_eval_tag(&paras->specularMap);
	o_Params.transparencyMap = *mi_eval_tag(&paras->transparencyMap);
	o_Params.g_ambient = *mi_eval_color(&paras->g_ambient);
	o_Params.g_diffuse = *mi_eval_color(&paras->g_diffuse);
	o_Params.g_midtone = *mi_eval_color(&paras->g_midtone);
	o_Params.g_specular = *mi_eval_color(&paras->g_specular);
	o_Params.g_colorTransition1 = *mi_eval_scalar(&paras->g_colorTransition1);
	o_Params.g_colorTransition2 = *mi_eval_scalar(&paras->g_colorTransition2);
	o_Params.g_smoothness = *mi_eval_scalar(&paras->g_smoothness);
	o_Params.g_specularPower = *mi_eval_scalar(&paras->g_specularPower);
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);
	o_Params.g_hasSpecular = *mi_eval_boolean(&paras->g_hasSpecular);

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
			  miBoolean bEnableSWL
			  ) 
{
	miColor out;

	miColor diff = make_color(0);
	if (!bEnableSWL)
	{
		diff = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
		if (diffuseEnvMap)
		{
			diff *= SampleEnvDiffuse(state, worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
		}	
	}
	else
	{
		/*miColor env = GetEnvironment(state);
		diff = g_diffuse*env;*/
	}

	out = diff;

    return out;
}

//--------------------------------------------------------------------
// ToonShader()
//--------------------------------------------------------------------
miColor ToonShader( miState* state, miVector sstt, miColor lightColor,
				  miVector worldEyeDir, miVector worldNormal, miVector lightDir,
				  miTag diffuseSampler, bool g_hasSpecular, miColor g_specular,
				  float g_specularPower, float g_colorTransition1, float g_colorTransition2,
				  miTag specularSampler, miTag glossSampler, float g_smoothness,
				  miColor g_midtone, miColor g_ambient, miColor g_diffuse,
				  bool bEnableDiffuse, bool bEnableSpecular)
{
	miColor rgb = make_color(0);

	miColor diffColor = lightColor * (bEnableDiffuse?1.0f:0.0f);
	if( diffuseSampler )
	{
		diffColor *= Tex2d( state, diffuseSampler, sstt );
	}

	float dif = saturate(dot(lightDir,worldNormal));

	float spec = 0;

	if( g_hasSpecular )
	{
		
		float spec = saturate( dot(worldNormal,normalize(lightDir - (worldEyeDir*-1))) );
		miColor specClr = g_specular * g_specularPower;
		float specTrans = g_colorTransition2;
		if( specularSampler)
		{
			specClr = Tex2d( state, specularSampler, sstt );
		}
		if( glossSampler )
		{
			miColor specTransColor = Tex2d( state, glossSampler, sstt );
			specTrans = specTransColor.r;
		}
		float interp = saturate( 1 - ((specTrans - spec) / (g_smoothness + NON_ZERO)) );
		rgb = specClr * lightColor * interp * (bEnableSpecular?1.0f:0.0f);

	}

	float trans0 = g_colorTransition1 * 0.25f;

	if( dif > g_colorTransition1 )
	{
		float interp = saturate(((dif - g_colorTransition1) / NonZeroVal((1.0f - g_colorTransition1) * g_smoothness)));
		diffColor *= lerp( g_midtone, g_diffuse, interp );
	}
	else if( dif > trans0 )
	{
		float interp = saturate(((dif - trans0) / NonZeroVal((g_colorTransition1 - trans0) * g_smoothness)));
		diffColor *= lerp( g_ambient, g_midtone, interp );
	}
	else
	{
		diffColor *= g_ambient;
	}

	rgb += diffColor;

	return rgb;
}

void LightLoop(miState* state,
			   int		m,
			   int		n_l,
			   miTag	*light,
			   sgpu_illum_Cartoon& o_Params,
			   miVector sstt,
			   miVector worldNormal,
			   miVector worldEyeDir,
			   miColor* result)
{
	/* Loop over all light sources */
	if (m == 4 || n_l) {
		for (mi::shader::LightIterator iter(state, light, n_l);
						!iter.at_end(); ++iter) {

			miColor sum = make_color(0);
			while (iter->sample()) {

				// Light info
				miColor	lightColor;
				iter->get_contribution(&lightColor);
				miVector lightDir = iter->get_direction();
				mi_vector_to_world(state,&lightDir,&lightDir);
				lightDir = normalize(lightDir);

				int size;
				sgpuLightData* info = (sgpuLightData*)mi_shaderstate_get( state, SGPU_LIGHT_DATA_NAME, &size );
				bool bMSPLight = (info) && !(o_Params.bEnableSWL && iter->get_current() == iter->get_number_of_lights()-1);
				bool AffectsDiffuse = (bMSPLight) ? info->AffectsDiffuse : true;
				bool AffectsSpecular = (bMSPLight) ? info->AffectsSpecular : true;

				if (!bMSPLight)
					lightColor = GetScaledIBL(lightColor);

				miColor diffuse = ToonShader( state, sstt, lightColor,
					normalize(worldEyeDir), worldNormal, normalize(lightDir),
					o_Params.diffuseMap, o_Params.g_hasSpecular, o_Params.g_specular,
					o_Params.g_specularPower, o_Params.g_colorTransition1, o_Params.g_colorTransition2,
					o_Params.specularMap, o_Params.glossMap, o_Params.g_smoothness,
					o_Params.g_midtone, o_Params.g_ambient, o_Params.g_diffuse,
					AffectsDiffuse, AffectsSpecular);

				sum += diffuse;
				
			}

			int samples = iter->get_number_of_samples();

			if (samples) {
				*result += sum / (miScalar)samples;
			}
		}
	}
}

extern "C" DLLEXPORT int sgpu_illum_Cartoon_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_illum_Cartoon(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_Cartoon *paras)
{

	miTag		*light;		/* tag of light instance */
	int			n_l;		/* number of light sources */
	int			i_l;		/* offset of light sources */
	int			m;			/* light mode: 0=all, 1=incl, 2=excl */

    /* check for illegal calls */
    if (state->type == miRAY_SHADOW || state->type == miRAY_DISPLACE ) {
		return(miFALSE);
	}

	struct sgpu_illum_Cartoon o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);
	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

	float g_transparency = o_Params.g_transparency * Tex2d(state, o_Params.transparencyMap, sstt).r;

	miColor sDiffColor = o_Params.g_diffuse;

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

	// SHADE NON-LIT SIDE
	mi_vector_neg(&state->normal);
	mi_vector_neg(&state->normal_geom);
	LightLoop(state,
			  m,
			  n_l,
			  light,
			  o_Params,
			  sstt,
			  worldNormal,
			  worldEyeDir,
			  result);

	// SHADE NON-LIT SIDE
	mi_vector_neg(&state->normal);
	mi_vector_neg(&state->normal_geom);
	LightLoop(state,
			  m,
			  n_l,
			  light,
			  o_Params,
			  sstt,
			  worldNormal,
			  worldEyeDir,
			  result);

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


