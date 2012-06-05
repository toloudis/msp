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
 *      sgpu_illum_SpecularFresnel()
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
#include "mi/math.h"

#include "Support.h"


// must match the _decl.mi file for this shader!
struct sgpu_illum_SpecularFresnel {

	// shader vars
	miTag tBase;
	miTag tSpecularMask;
	miTag tSpecularShift;
	miTag tAlpha;
	miColor		g_hairBaseColor;
	miColor		g_specularColor0;
	miColor		g_specularColor1;
	miScalar	g_reflectivity;
	miScalar	g_specularExp0;
	miScalar	g_specularExp1;
	miScalar	g_specularShift0;
	miScalar	g_specularShift1;
	miScalar	g_transparency;

	// env
	miTag		diffuseEnvMap;
	miTag		specularEnvMap;
	miColor		g_envDiffuseColor;
	miColor		g_envSpecularColor;
	miScalar	g_diffuseFactor;
	miScalar	g_diffuseEnvAngle;
	miScalar	g_specularFactor;
	miScalar	g_specularEnvAngle;

	// refl
	miBoolean	g_isPlanar;

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

	int		mode;           /* light mode: 0..2 */
	int		i_light;	/* index of first light */
	int		n_light;	/* number of lights */
	miTag	light[1];	/* list of lights */
};

void EvalParams(miState *state,
				const sgpu_illum_SpecularFresnel* paras,
				sgpu_illum_SpecularFresnel& o_Params)
{
	// shader params
	o_Params.tBase = *mi_eval_tag(&paras->tBase);
	o_Params.tSpecularMask = *mi_eval_tag(&paras->tSpecularMask);
	o_Params.tSpecularShift = *mi_eval_tag(&paras->tSpecularShift);
	o_Params.tAlpha = *mi_eval_tag(&paras->tAlpha);
	o_Params.g_hairBaseColor = *mi_eval_color(&paras->g_hairBaseColor);
	o_Params.g_specularColor0 = *mi_eval_color(&paras->g_specularColor0);
	o_Params.g_specularColor1 = *mi_eval_color(&paras->g_specularColor1);
	o_Params.g_reflectivity = *mi_eval_scalar(&paras->g_reflectivity);
	o_Params.g_specularExp0 = *mi_eval_scalar(&paras->g_specularExp0);
	o_Params.g_specularExp1 = *mi_eval_scalar(&paras->g_specularExp1);
	o_Params.g_specularShift0 = *mi_eval_scalar(&paras->g_specularShift0);
	o_Params.g_specularShift1 = *mi_eval_scalar(&paras->g_specularShift1);
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);

	// env
	o_Params.diffuseEnvMap = *mi_eval_tag(&paras->diffuseEnvMap);
	o_Params.specularEnvMap = *mi_eval_tag(&paras->specularEnvMap);
	o_Params.g_envDiffuseColor = *mi_eval_color(&paras->g_envDiffuseColor);
	o_Params.g_envSpecularColor = *mi_eval_color(&paras->g_envSpecularColor);
	o_Params.g_diffuseFactor = *mi_eval_scalar(&paras->g_diffuseFactor);
	o_Params.g_diffuseEnvAngle = *mi_eval_scalar(&paras->g_diffuseEnvAngle);
	o_Params.g_specularFactor = *mi_eval_scalar(&paras->g_specularFactor);
	o_Params.g_specularEnvAngle = *mi_eval_scalar(&paras->g_specularEnvAngle);

	// refl
	o_Params.g_isPlanar = *mi_eval_boolean(&paras->g_isPlanar);

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
			  miScalar g_emissiveIntensity,
			  float bumpScale, 
			  float refrFactor, 
			  float reflFactor,
			  miTag reflectFactorSampler,
			  float fresnelBias, 
			  float fresnelPower, 
			  float IOR,
			  float reflLOD, 
			  bool reflEnable,
			  miBoolean bEnableSWL
			  ) 
{
	miColor out;

	//early alpha test
	//OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	//if( g_AlphaTestRef >= OUT.col.a ) discard;

	//fetch bump normal
	//miVector worldEyeDir = state->dir;// = normalize( IN.V.WorldPos - g_eyePos.xyz);
	//miVector worldNormal = state->normal;//GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );

	//miColor rr = make_color(0);
	miVector bumpNormal = worldNormal;

	//float4 refl = 0;
	//if (hasCubeMap)
	//{    		
	//	refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeMap, g_reflMapAngle*PI_DIV_180 , reflBlur );
	//	refl *= g_reflectivity;
	//	refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
	//	refl = Tex2DCombine(hasReflectFactorMap, reflectFactorMap, IN.V.TexCoord0, refl);
	//}

	/*rr = GetReflection(state, sstt, bumpNormal,
					   worldNormal, worldEyeDir, 
					   bumpScale, refrFactor, reflFactor,
					   reflectFactorSampler,
					   fresnelBias, fresnelPower, IOR,
					   reflLOD, reflEnable);*/

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

	out = g_emissive*g_emissiveIntensity + spec + diff;
	//OUT.col.rgb = g_IsolateReflection ? refl.rgb : g_emissive.rgb * g_emissiveIntensity + spec.rgb + refl.rgb + diff.rgb;
	//OUT.col.rgb *= OUT.col.a;	//premul alpha

    return out;
}

extern "C" DLLEXPORT int sgpu_illum_SpecularFresnel_version(void) {return(1);}

// to shift the specular highlight along the length of the hair, 
// we nudge the tangent along the direction of the normal.
// assumes T pointing from root to tip.
// positive nudge moves hilight toward root, negative - toward tip
// get shift value from texture to break up uniform look over all hair patches.
miVector ShiftTangent(const miVector& T, const miVector& N, float shift)
{
	miVector shiftedT = T + shift*N;
	return normalize(shiftedT);
}

// uses half angle vector. could use refl vector, at cost of complexity.
float StrandSpecular(const miVector& T, const miVector& H, float exponent)
{
	float dotTH = dot(T,H);
	float sinTH = saturate(sqrt(1.0f - dotTH*dotTH));
	float dirAtten = mi::math::smoothstep(-1.0f, 0.0f, dotTH);
//	return dirAtten * pow(max(0.001,sinTH), max(0.001,exponent));
	return sinTH == 0 ? 0 : dirAtten * pow(sinTH, exponent);
}

float HairDiffuseTerm(const miVector& N, const miVector& L)
{
    return saturate(0.75f * dot(N, L) + 0.25f);
}

miColor HairDiffuseContrib(const miVector& normal, const miVector& lightVec,
	const miColor& lightColor, const miColor& g_hairBaseColor)
{
	// diffuse lighting: the lerp shifts the shadow boundary for a softer look
	float d = saturate(mi::math::lerp(0.25f,1.0f, dot(normal, lightVec)));
	//float d = saturate(dot(normal, lightVec));
	miColor diffuse = d * lightColor * g_hairBaseColor;
	
	return diffuse;
}
miColor HairSpecularContrib(const miVector& tangent, const miVector& normal, const miVector& halfVec,
	const miVector& uv,
	miState* state, sgpu_illum_SpecularFresnel* params)
{
	// shift tangents
	float shiftTex = Tex2dReplace(state, params->tSpecularShift, make_color(0.5f), uv).r - 0.5f;
	miVector t1 = ShiftTangent(tangent, normal, params->g_specularShift0 + shiftTex);
	miVector t2 = ShiftTangent(tangent, normal, params->g_specularShift1 + shiftTex);

	// 2 hilights of different colors, specular exponents, and differently shifted tangents.

	// add 2nd specular term, modulated with noise texture
	float specMask = Tex2dReplace(state, params->tSpecularMask, make_color(1), uv).r; // approximate sparkles using texture
	
	// specular lighting
	miColor specular = (params->g_specularColor0 * StrandSpecular(t1, halfVec, params->g_specularExp0) + 
					   params->g_specularColor1 * StrandSpecular(t2, halfVec, params->g_specularExp1)) * specMask;
	
	return specular;
}

miColor HairLighting(const miVector& tangent, const miVector& normal, const miVector& lightVec,
	const miVector& halfVec, const miVector& uv, float ambOcc, 
	const miColor& lightDiffColor, const miColor& lightSpecColor,
	miState* state,	sgpu_illum_SpecularFresnel* params)
{
	miColor diffuse = HairDiffuseContrib(normal, lightVec, lightDiffColor, params->g_hairBaseColor);

	miColor specular = HairSpecularContrib(tangent, normal, halfVec, uv, state, params);

    // specular attenuation for hair facing away from light
    float specularAttenuation = saturate(1.75f * dot(normal, lightVec) + 0.5f);
	
	miColor base = Tex2dReplace(state, params->tBase, make_color(1), uv);
	 	
	// final color assembly
	miColor o = make_color(0);
    o = diffuse * base;

    o += specular * base * specularAttenuation * lightSpecColor;

	float alpha = Tex2dReplace(state, params->tAlpha, make_color(0), uv).r;
	o.a = 1.0f - alpha;
	o.a *= params->g_transparency;
	return o;
}

void LightLoop(miState* state,
			   int		m,
			   int		n_l,
			   miTag	*light,
			   sgpu_illum_SpecularFresnel& o_Params,
			   miVector sstt,
			   miVector worldNormal,
			   miVector worldEyeDir,
			   miVector worldTan,
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
				
				miColor lightColorDiff = (AffectsDiffuse ? 1 : 0) * lightColor;
				miColor lightColorSpec = (AffectsSpecular ? 1 : 0) * lightColor;

				if (!bMSPLight)
					lightColor = GetScaledIBL(lightColor);

				// Get color
				miColor col = HairLighting(worldTan, worldNormal, lightDir,
					normalize(lightDir + worldEyeDir), sstt, 1.0,
					lightColorDiff, lightColorSpec,
					state, &o_Params);

				sum += col;				
			}

			int samples = iter->get_number_of_samples();
			if (samples) {
				*result += sum / (miScalar)samples;
			}
		}
	}
}

extern "C" DLLEXPORT miBoolean sgpu_illum_SpecularFresnel(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_SpecularFresnel *paras)
{

	miTag		*light;		/* tag of light instance */
	int			n_l;		/* number of light sources */
	int			i_l;		/* offset of light sources */
	int			m;			/* light mode: 0=all, 1=incl, 2=excl */

    /* check for illegal calls */
    if (state->type == miRAY_SHADOW || state->type == miRAY_DISPLACE ) {
		return(miFALSE);
	}

	struct sgpu_illum_SpecularFresnel o_Params;
	EvalParams(state, paras, o_Params);

	// UV transform
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);
	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

	miVector worldTangent = state->bump_x_list[0];
	miVector worldBiNormal = state->bump_y_list[0];
	mi_vector_to_world(state,&worldTangent,&worldTangent);
	mi_vector_to_world(state,&worldBiNormal,&worldBiNormal);
	worldTangent = normalize(worldTangent);
	worldBiNormal = normalize(worldBiNormal);

	// the (0,1,0) direction in tangent space:
	miVector worldTan = make_vector(worldTangent.y, worldBiNormal.y, worldNormal.y);//float3(IN.V.WorldTan.X.y, IN.V.WorldTan.Y.y, IN.V.WorldTan.Z.y);
	worldTan = normalize(worldTan);
 
	float alpha = Tex2dReplace(state, o_Params.tAlpha, make_color(0), sstt).r;
	float g_transparency = 1.0f - alpha;
	g_transparency *= o_Params.g_transparency;

	miColor sDiffColor = Tex2dCombine(state, o_Params.tBase, o_Params.g_hairBaseColor, sstt);

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
					 o_Params.g_hairBaseColor,
					 o_Params.tBase,
					 o_Params.g_reflectivity,
					 make_color(0),
					 0,
					 1, /*bumpScale*/
					 0,					 
					 0,
					 0, 
					 0, 
					 0, 
					 0,
					 1, 
					 true,
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
			  worldTan,
			  result);

	// SHADE LIT SIDE
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
			  worldTan,
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


