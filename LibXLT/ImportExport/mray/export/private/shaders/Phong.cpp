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
 *      sgpu_illum_Phong()
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
struct sgpu_illum_Phong {

	// shader vars
	miTag		cubeMap;
	miTag		reflectFactorMap;
	miColor		g_diffuse;	
	miTag		diffuseMap;
	miColor		g_emissive;	
	miTag		emissiveMap;
	miScalar	g_emissiveIntensity;
	miColor		g_specular;	
	miTag		specularMap;
	miTag		glossMap;
	miScalar	g_specularPower;
	miScalar	g_shininess;
	miScalar	g_reflectivity;
	miScalar	fresnelPower;
	miScalar	fresnelBias;
	miScalar	reflBlur;
	miColor		g_ambient;
	miScalar	g_transparency;
	miTag		transparencyMap;
	miScalar	g_reflMapAngle;

	// env
	miTag		diffuseEnvMap;
	miColor		g_envDiffuseColor;
	miScalar	g_diffuseFactor;
	miScalar	g_diffuseEnvAngle;
	miTag		specularEnvMap;
	miColor		g_envSpecularColor;
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

	int			mode;       /* light mode: 0..2 */
	int			i_light;	/* index of first light */
	int			n_light;	/* number of lights */
	miTag		light[1];	/* list of lights */
};

void EvalParams(miState *state,
				const sgpu_illum_Phong* paras,
				sgpu_illum_Phong& o_Params)
{ 
	// shader params
	o_Params.cubeMap = *mi_eval_tag(&paras->cubeMap);
	o_Params.reflectFactorMap = *mi_eval_tag(&paras->reflectFactorMap);
	o_Params.g_diffuse = *mi_eval_color(&paras->g_diffuse);
	o_Params.diffuseMap = *mi_eval_tag(&paras->diffuseMap);
	o_Params.g_emissive = *mi_eval_color(&paras->g_emissive);
	o_Params.emissiveMap = *mi_eval_tag(&paras->emissiveMap);
	o_Params.g_emissiveIntensity = *mi_eval_scalar(&paras->g_emissiveIntensity);
	o_Params.g_specular =  *mi_eval_color(&paras->g_specular);
	o_Params.specularMap = *mi_eval_tag(&paras->specularMap);
	o_Params.g_specularPower = *mi_eval_scalar(&paras->g_specularPower);
	o_Params.glossMap = *mi_eval_tag(&paras->glossMap);
	o_Params.g_shininess = *mi_eval_scalar(&paras->g_shininess);
	o_Params.g_reflectivity = *mi_eval_scalar(&paras->g_reflectivity);
	o_Params.fresnelPower = *mi_eval_scalar(&paras->fresnelPower);
	o_Params.fresnelBias = *mi_eval_scalar(&paras->fresnelBias);
	o_Params.reflBlur = *mi_eval_scalar(&paras->reflBlur);
	o_Params.g_ambient = *mi_eval_color(&paras->g_ambient);
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);
	o_Params.transparencyMap = *mi_eval_tag(&paras->transparencyMap);
	o_Params.g_reflMapAngle = *mi_eval_scalar(&paras->g_reflMapAngle);

	// env
	o_Params.diffuseEnvMap = *mi_eval_tag(&paras->diffuseEnvMap);	
	o_Params.g_envDiffuseColor = *mi_eval_color(&paras->g_envDiffuseColor);
	o_Params.g_diffuseFactor = *mi_eval_scalar(&paras->g_diffuseFactor);
	o_Params.g_diffuseEnvAngle = *mi_eval_scalar(&paras->g_diffuseEnvAngle);
	o_Params.specularEnvMap = *mi_eval_tag(&paras->specularEnvMap);
	o_Params.g_envSpecularColor = *mi_eval_color(&paras->g_envSpecularColor);
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
			  miTag diffuseMap,
			  miScalar g_reflectivity,
			  miColor g_emissive,
			  miTag emissiveMap,
			  miScalar g_emissiveIntensity,
			  miTag cubeMap,
			  miScalar reflBlur,
			  miScalar g_reflMapAngle,
			  miScalar fresnelBias,
			  miScalar fresnelPower,
			  miTag reflectFactorMap,
			  miBoolean bEnableSWL
			  ) 
{
	miColor out;

	miColor refl = make_color(0);
	if (cubeMap)
	{    
		/*miVector incid = worldNormal;
		miVector norm = worldEyeDir;
		mi_vector_to_world(state,&incid,&incid);
		mi_vector_to_world(state,&norm,&norm);
		incid = normalize(incid);
		norm = normalize(norm);*/

		//Print("worldEyeDir",worldEyeDir);
		//Print("incid",incid);

		//Print("worldNormal",worldNormal);
		//Print("norm",norm);

		refl = SampleEnvironmentLOD(state,(worldEyeDir*-1), worldNormal, cubeMap, g_reflMapAngle , reflBlur );
		refl *= g_reflectivity;
		refl *= saturate(fastFresnel(dot((worldEyeDir), worldNormal), fresnelBias, fresnelPower));

		//Print("fresnelBias",fresnelBias);
		//Print("fresnelPower",fresnelPower);
		//Print("#########");
		refl = Tex2dCombine(state, reflectFactorMap, refl, sstt);
	}
	PutRefl(&refl, state, true);

	miColor spec = make_color(0), diff = make_color(0);
	if (!bEnableSWL)
	{
		spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
		if (specularEnvMap) 
		{
			spec *= SampleEnvironment(state, worldEyeDir*-1, worldNormal, specularEnvMap, g_specularEnvAngle);
		}

		diff = Tex2dCombine(state, diffuseMap, g_diffuse*g_envDiffuseColor*g_diffuseFactor, sstt);
		if (diffuseEnvMap)
		{
			diff *= SampleEnvDiffuse(state, worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
		}	
	}
	else
	{
		//miColor env = GetEnvironment(state);

		////spec = g_reflectivity*g_specularFactor*make_color(0);
		//diff = Tex2dCombine(state, diffuseMap, g_diffuse*env, sstt);
	}

	miColor emissive = Tex2dCombine(state, emissiveMap, g_emissive*g_emissiveIntensity, sstt);
	out = emissive + spec + diff + refl;

    return out;
}

extern "C" DLLEXPORT int sgpu_illum_Phong_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_illum_Phong(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_Phong *paras)
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

	struct sgpu_illum_Phong o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);
	float g_transparency = o_Params.g_transparency * Tex2d(state, o_Params.transparencyMap, sstt).r;
	miColor sDiffColor = Tex2dCombine( state, o_Params.diffuseMap , o_Params.g_diffuse, sstt );

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);

	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

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
					 o_Params.diffuseMap,
					 o_Params.g_reflectivity,
					 o_Params.g_emissive,
					 o_Params.emissiveMap,
					 o_Params.g_emissiveIntensity,
					 o_Params.cubeMap,
					 o_Params.reflBlur,
					 o_Params.g_reflMapAngle,
					 o_Params.fresnelBias,
					 o_Params.fresnelPower,
					 o_Params.reflectFactorMap,
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

				if (AffectsDiffuse)
				{
					miColor diffuse = PhongDiffuse(worldNormal, lightDir, lightColor, sDiffColor);
					sum += diffuse;
				}
				if (AffectsSpecular)
				{
					miColor sSpecColor = Tex2dCombine( state, o_Params.specularMap , o_Params.g_specular, sstt );
					miColor sSpecPower = Tex2dCombine( state, o_Params.glossMap , make_color(o_Params.g_shininess), sstt );

					// Get color
					miColor specular = PhongSpecular(worldNormal, lightDir, worldEyeDir, lightColor, sSpecColor, sSpecPower.r) * o_Params.g_specularPower;

					sum += specular;
				}
				
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

	// FG
	//miColor indirect;
	//mi_compute_irradiance(&indirect, state);
	/*miIrrad_options irrOpt;
	miIRRAD_DEFAULT(&irrOpt, state);

	irrOpt.finalgather_maxradius = 1.0f;
	irrOpt.finalgather_minradius = 0.1;
	irrOpt.finalgather_filter = 0;*/
	//mi_compute_avg_radiance(&indirect, state, 'f', &irrOpt);
	//*result += indirect * sDiffColor;//  * PI;
	//mi_fb_put(state,1,&indirect);

	// transparency blending
	if (g_transparency < 1)
	{
		miColor trans;
		mi_trace_transparent(&trans, state);
        *result = (*result)*g_transparency + trans*(1.0f - g_transparency);
	}

	return(miTRUE);
}


