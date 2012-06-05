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
 *      sgpu_illum_SubSurfaceScatter_wBlinnSpecular()
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
struct sgpu_illum_SubSurfaceScatter_wBlinnSpecular {
	// shader vars
	miTag diffTex;
	miTag transMapIn;
	miTag transMapOut;
	miTag transMapBack;
	miTag transTex;
	miTag specularMap;
	miTag iorMap;
	miTag glossMap;
	miTag microTex;
	miTag transparencyTex;
	miTag cubeTex;
	miTag reflectFactorTex;
	miColor g_transColIn;
	miColor g_transColOut;
	miColor g_transColBack;
	miColor g_specular;
	miScalar g_transMultiplier;
	miScalar g_transRampOff;
	miScalar g_specularPower;
	miScalar g_IOR;
	miScalar g_shininess;
	miScalar g_microScale;
	miScalar g_transparency;
	miScalar g_reflectivity;
	miScalar fresnelPower;
	miScalar fresnelBias;
	miScalar reflBlur;
	miScalar g_reflMapAngle;

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

	int		mode;           /* light mode: 0..2 */
	int		i_light;	/* index of first light */
	int		n_light;	/* number of lights */
	miTag		light[1];	/* list of lights */
};

void EvalParams(miState *state,
				const sgpu_illum_SubSurfaceScatter_wBlinnSpecular* paras,
				sgpu_illum_SubSurfaceScatter_wBlinnSpecular& o_Params)
{
	// shader params
	o_Params.diffTex = *mi_eval_tag(&paras->diffTex);
	o_Params.transMapIn = *mi_eval_tag(&paras->transMapIn);
	o_Params.transMapOut = *mi_eval_tag(&paras->transMapOut);
	o_Params.transMapBack = *mi_eval_tag(&paras->transMapBack);
	o_Params.transTex = *mi_eval_tag(&paras->transTex);
	o_Params.specularMap = *mi_eval_tag(&paras->specularMap);
	o_Params.iorMap = *mi_eval_tag(&paras->iorMap);
	o_Params.glossMap = *mi_eval_tag(&paras->glossMap);
	o_Params.microTex = *mi_eval_tag(&paras->microTex);
	o_Params.transparencyTex = *mi_eval_tag(&paras->transparencyTex);
	o_Params.cubeTex = *mi_eval_tag(&paras->cubeTex);
	o_Params.reflectFactorTex = *mi_eval_tag(&paras->reflectFactorTex);
	o_Params.g_transColIn = *mi_eval_color(&paras->g_transColIn);
	o_Params.g_transColOut = *mi_eval_color(&paras->g_transColOut);
	o_Params.g_transColBack = *mi_eval_color(&paras->g_transColBack);
	o_Params.g_specular = *mi_eval_color(&paras->g_specular);
	o_Params.g_transMultiplier = *mi_eval_scalar(&paras->g_transMultiplier);
	o_Params.g_transRampOff = *mi_eval_scalar(&paras->g_transRampOff);
	o_Params.g_specularPower = *mi_eval_scalar(&paras->g_specularPower);
	o_Params.g_IOR = *mi_eval_scalar(&paras->g_IOR);
	o_Params.g_shininess = *mi_eval_scalar(&paras->g_shininess);
	o_Params.g_microScale = *mi_eval_scalar(&paras->g_microScale);
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);
	o_Params.g_reflectivity = *mi_eval_scalar(&paras->g_reflectivity);
	o_Params.fresnelPower = *mi_eval_scalar(&paras->fresnelPower);
	o_Params.fresnelBias = *mi_eval_scalar(&paras->fresnelBias);
	o_Params.reflBlur = *mi_eval_scalar(&paras->reflBlur);
	o_Params.g_reflMapAngle = *mi_eval_scalar(&paras->g_reflMapAngle);

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
			  miTag diffuseMap,
			  miScalar g_reflectivity,
			  miTag cubeMap,
			  miScalar reflBlur,
			  miScalar g_reflMapAngle,
			  miScalar fresnelBias,
			  miScalar fresnelPower,
			  miTag reflectFactorMap,
			  miTag TransMapIn,
			  miColor g_transColIn,
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

	miColor refl = make_color(0);
	if (cubeMap)
	{    
		refl = SampleEnvironmentLOD(state,(worldEyeDir*-1), worldNormal, cubeMap, g_reflMapAngle , reflBlur );
		refl *= g_reflectivity;
		refl *= saturate(fastFresnel(dot((worldEyeDir), worldNormal), fresnelBias, fresnelPower));
		refl = Tex2dCombine(state, reflectFactorMap, refl, sstt);
	}
	PutRefl(&refl, state, true);


	miColor spec = make_color(0), diff = make_color(0);
	miColor transColMapIn = Tex2dCombine(state, TransMapIn, g_transColIn, sstt);
	if (!bEnableSWL)
	{
		spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
		if (specularEnvMap) 
		{
			spec *= SampleEnvironment(state, worldEyeDir*-1, worldNormal, specularEnvMap, g_specularEnvAngle);
		}
		
		diff = Tex2dCombine(state, diffuseMap, transColMapIn*g_envDiffuseColor*g_diffuseFactor, sstt);
		if (diffuseEnvMap)
		{
			diff *= SampleEnvDiffuse(state, worldNormal, diffuseEnvMap, g_diffuseEnvAngle);
		}
	}
	else
	{
		//miColor env = GetEnvironment(state);

		////spec = g_reflectivity*g_specularFactor*make_color(0);
		//diff = Tex2dCombine(state, diffuseMap, transColMapIn*env, sstt);
	}

	out = spec + refl + diff;

    return out;
}

extern "C" DLLEXPORT int sgpu_illum_SubSurfaceScatter_wBlinnSpecular_version(void) {return(1);}

//=======================Translucency lighting model=======================
miColor TransPass(	float DotLN,
			const miVector& TexUV,
			miState* state, sgpu_illum_SubSurfaceScatter_wBlinnSpecular* params)
{
	miColor transSample = Tex2dReplace(state, params->transTex, make_color(0), TexUV);
	
	static const miColor one = make_color(1);
	miColor Translucence 	= smoothstep(-params->g_transRampOff  * transSample,one,make_color(abs(DotLN))) 
					;//- smoothstep(one,one,DotLN); // or should it be step(one, DotLN);//???
	
	miColor transColMapIn = Tex2dCombine(state, params->transMapIn, params->g_transColIn, TexUV);
	miColor transColMapOut = Tex2dCombine(state, params->transMapOut, params->g_transColOut, TexUV);
	miColor transColMapBack = Tex2dCombine(state, params->transMapBack, params->g_transColBack, TexUV);
	//float4 Colourise		= lerp(g_transColBack, lerp(g_transColOut * g_transMultiplier,g_transColIn,DotLN),Translucence);
	miColor Colourise		= lerp(transColMapBack, lerp(transColMapOut * params->g_transMultiplier,transColMapIn,DotLN),Translucence);
	
	return (Colourise * Translucence);
}

//================================Pixel shader - Complete================================
miColor SkinShader(const miVector& TexUV,  
	const miVector& WN, // normalize(world space normal)
	const miVector& EV, //= normalize(IN.WorldEyeDir);     //(world space)
	const miVector& LV, //= normalize(IN.LightVector.xyz); //(world space)
	const miColor& LightColourDiff, const miColor& LightColourSpec,
	miState* state, sgpu_illum_SubSurfaceScatter_wBlinnSpecular* params)
{
	float DotLN = dot(LV,WN);

    miColor a = Tex2dReplace(state, params->diffTex, make_color(1), TexUV);
	miColor b 			= TransPass(DotLN,TexUV,state, params);
    
//	float4 a 			= tex2D(DiffMap,TexUV);	
//	float4 BaseLighting	= ((1 - a) * (a*b) + a * (1 - (1 - a) * (1 - b))) * b;

	miColor BaseLighting = (a * b)*(a + b + b - 2*a*b);
	
	miColor sSpecColor = Tex2dCombine(state, params->specularMap, params->g_specular, TexUV);
	float sSpecPower  = Tex2dCombine(state, params->glossMap, make_color(params->g_shininess), TexUV).r;	
	float ior		  = Tex2dCombine(state, params->iorMap, make_color(params->g_IOR-1), TexUV).r + 1;
	
	miColor specularComponent = BlinnSpecular(state, WN, LV, EV, LightColourSpec, sSpecColor, 
											 sSpecPower, ior, 0, params->g_specularPower);

	miColor LightingOutput = BaseLighting * LightColourDiff + 
							specularComponent * LightColourSpec;
				
	return LightingOutput;
} 

void LightLoop(miState* state,
			   int		m,
			   int		n_l,
			   miTag	*light,
			   sgpu_illum_SubSurfaceScatter_wBlinnSpecular& o_Params,
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
				
				miColor lightColorDiff = (AffectsDiffuse ? 1 : 0) * lightColor;
				miColor lightColorSpec = (AffectsSpecular ? 1 : 0) * lightColor;

				if (!bMSPLight)
					lightColor = GetScaledIBL(lightColor);

				// Get color
				miColor col = SkinShader(sstt, normalize(worldNormal),
					worldEyeDir, lightDir, lightColorDiff, lightColorSpec, 
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

extern "C" DLLEXPORT miBoolean sgpu_illum_SubSurfaceScatter_wBlinnSpecular(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_SubSurfaceScatter_wBlinnSpecular *paras)
{

	miTag		*light;		/* tag of light instance */
	int			n_l;		/* number of light sources */
	int			i_l;		/* offset of light sources */
	int			m;			/* light mode: 0=all, 1=incl, 2=excl */
	int			samples;	/* # of samples taken */
	miColor		sum;		/* summed sample colors */
	miColor		lightColorDiff, lightColorSpec;
	int size;

    /* check for illegal calls */
    if (state->type == miRAY_SHADOW || state->type == miRAY_DISPLACE ) {
		return(miFALSE);
	}

	struct sgpu_illum_SubSurfaceScatter_wBlinnSpecular o_Params;
	EvalParams(state, paras, o_Params);

	// UV transform
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);
	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);
 
	miColor	transparencyMap_color = Tex2d(state, o_Params.transparencyTex, sstt);
	float g_transparency = o_Params.g_transparency * transparencyMap_color.r;

	miColor sDiffColor = Tex2dCombine(state, o_Params.diffTex, o_Params.g_transColIn, sstt);

	// IBL_PS
	miColor ibl = iblPS( state,
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
					 o_Params.g_transColIn,
					 o_Params.diffTex,
					 o_Params.g_reflectivity,
					 o_Params.cubeTex,
					 o_Params.reflBlur,
					 o_Params.g_reflMapAngle,
					 o_Params.fresnelBias,
					 o_Params.fresnelPower,
					 o_Params.reflectFactorTex,
					 o_Params.transMapIn,
					 o_Params.g_transColIn,
					 o_Params.bEnableSWL);

	*result = ibl;

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


