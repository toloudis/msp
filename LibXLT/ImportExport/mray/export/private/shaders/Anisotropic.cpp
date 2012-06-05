/*****************************************************************************
**  Anisotropic.fx
**
**      Anisotropic (Torrance-Sparrow) shader
**
**	Studio GPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

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
struct sgpu_illum_Anisotropic {
	miBoolean	g_isPlanar;
	miTag		cubeMap;
	miTag		g_planarMap;
	miColor		g_diffuse;
	miTag		diffuseMap;
	miColor		g_emissive;
	miTag		emissiveMap;
	miScalar	g_emissiveIntensity;
	miColor		g_specular;
	miTag		specularMap;
	miScalar	SpecularColorPower;
	miTag		specularColorPowerMap;
	miScalar	g_shininess;
	miTag		shininessMap;
	miScalar	g_UPower;
	miScalar	g_VPower;
	miTag		anisotropyMap;
	miScalar	g_transparency;
	miTag		transparencyMap;
	miScalar	g_reflectivity;
	miScalar	fresnelPower;
	miScalar	fresnelBias;
	miScalar	reflScale;
	miScalar	g_reflectionEnvAngle;
	miTag		CubeReflMap;	
	miTag		reflectFactorMap;
	miScalar	g_specularBias;
	miScalar	g_specularBias2;
	//miTag		glowMask;

	// additional params
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
	miBoolean	reflEnable;

	// UV transform
    miScalar u_scale;
    miScalar v_scale;
    miScalar u_offset;
    miScalar v_offset;
	miScalar uv_rotation;

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
				const sgpu_illum_Anisotropic* paras,
				sgpu_illum_Anisotropic& o_Params)
{ 
	// shader params
	o_Params.g_isPlanar = *mi_eval_boolean(&paras->g_isPlanar);
	o_Params.cubeMap = *mi_eval_tag(&paras->cubeMap);
	o_Params.g_planarMap = *mi_eval_tag(&paras->g_planarMap);
	o_Params.g_diffuse = *mi_eval_color(&paras->g_diffuse);
	o_Params.diffuseMap = *mi_eval_tag(&paras->diffuseMap);
	o_Params.g_emissive = *mi_eval_color(&paras->g_emissive);
	o_Params.emissiveMap = *mi_eval_tag(&paras->emissiveMap);
	o_Params.g_emissiveIntensity = *mi_eval_scalar(&paras->g_emissiveIntensity);
	o_Params.g_specular = *mi_eval_color(&paras->g_specular);
	o_Params.specularMap = *mi_eval_tag(&paras->specularMap);
	o_Params.SpecularColorPower = *mi_eval_scalar(&paras->SpecularColorPower);
	o_Params.specularColorPowerMap = *mi_eval_tag(&paras->specularColorPowerMap);
	o_Params.g_shininess = *mi_eval_scalar(&paras->g_shininess);
	o_Params.shininessMap = *mi_eval_tag(&paras->shininessMap);
	o_Params.g_UPower = *mi_eval_scalar(&paras->g_UPower);
	o_Params.g_VPower = *mi_eval_scalar(&paras->g_VPower);
	o_Params.anisotropyMap = *mi_eval_tag(&paras->anisotropyMap);
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);
	o_Params.transparencyMap = *mi_eval_tag(&paras->transparencyMap);
	o_Params.g_reflectivity = *mi_eval_scalar(&paras->g_reflectivity);
	o_Params.fresnelPower = *mi_eval_scalar(&paras->fresnelPower);
	o_Params.fresnelBias = *mi_eval_scalar(&paras->fresnelBias);
	o_Params.reflScale = *mi_eval_scalar(&paras->reflScale);
	o_Params.g_reflectionEnvAngle = *mi_eval_scalar(&paras->g_reflectionEnvAngle);
	o_Params.CubeReflMap = *mi_eval_tag(&paras->CubeReflMap);
	o_Params.reflectFactorMap = *mi_eval_tag(&paras->reflectFactorMap);
	o_Params.g_specularBias = *mi_eval_scalar(&paras->g_specularBias);
	o_Params.g_specularBias2 = *mi_eval_scalar(&paras->g_specularBias2);

	// env
	o_Params.diffuseEnvMap = *mi_eval_tag(&paras->diffuseEnvMap);	
	o_Params.g_envDiffuseColor = *mi_eval_color(&paras->g_envDiffuseColor);
	o_Params.g_diffuseFactor = *mi_eval_scalar(&paras->g_diffuseFactor);
	o_Params.g_diffuseEnvAngle = *mi_eval_scalar(&paras->g_diffuseEnvAngle);
	o_Params.specularEnvMap = *mi_eval_tag(&paras->specularEnvMap);
	o_Params.g_envSpecularColor = *mi_eval_color(&paras->g_envSpecularColor);
	o_Params.g_specularFactor = *mi_eval_scalar(&paras->g_specularFactor);
	o_Params.g_specularEnvAngle = *mi_eval_scalar(&paras->g_specularEnvAngle);

	//refl
	o_Params.reflEnable = *mi_eval_boolean(&paras->reflEnable);

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
			  miTag CubeReflMap,
			  miTag reflectFactorMap,
			  miScalar g_reflectionEnvAngle,
			  miScalar fresnelBias,
			  miScalar fresnelPower,
			  miBoolean bEnableSWL
			  ) 
{
	miColor out;

	//early alpha test
	/*OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;*/

	float ndv = dot(worldEyeDir,worldNormal);
	float fresnel = saturate(fastFresnel( ndv, fresnelBias, fresnelPower ));
	miVector reflVect = reflect( worldEyeDir, worldNormal );
	miColor refl = make_color(0);

	if( CubeReflMap )
	{			
		miVector rotReflVect = rotateAboutY(reflVect, g_reflectionEnvAngle-180);
		//refl = CubeReflMap.SampleLevel( AnisoClampSampler, CartesianToPolar( rotReflVect ), reflScale*fresnel ) * g_reflectivity;
		refl = Tex2d(state,CubeReflMap,CartesianToPolar( rotReflVect )) * g_reflectivity;
	}

	refl = Tex2dCombine(state, reflectFactorMap, refl, sstt);
	refl = refl * fresnel;
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
	out =  emissive + spec + diff + refl;
	//OUT.col.rgb = g_IsolateReflection ? refl.rgb : g_emissive.rgb * g_emissiveIntensity + spec.rgb + refl.rgb + diff.rgb;
	//OUT.col.rgb *= OUT.col.a;	//premul alpha

    return out;
}

float WardAnisotropicSpecular(miVector normal, miVector lightDir, miVector eyeDir, miVector Tangent, miVector Binormal, float Ax, float Ay )
{
	//inputs
	//V = surface to eye unit vector
	//L = surface to light unit vector
	//H = half angle
	//N = Surface normal unit vector
	//X = Tangent
	//Y = Binormal
	//Ax = x anisotropy
	//Ay = y anisotropy
	
	miVector V = eyeDir;
	miVector L = lightDir;
	miVector H = normalize(V + L);
	miVector N = normal;
	miVector X = Tangent;
	miVector Y = Binormal;
	
	//Ay = clamp(Ay, 0, 90) * PI_DIV_180;
	//float wx = max(Ax * sin(Ay), AThres);
	//float wy = max(Ax * cos(Ay), AThres);
	
	// Set Ax, Ay make sure wx, wy they are not going to zero
	Ay = mi::math::clamp(Ay, 0.1f, 89.9f) * 3.1415f / 180.0f;//PI_DIV_180;
	Ax = mi::math::clamp(Ax, 0.0f, 1.0f);	
	
	float wx = 0.5f - 0.5f * Ax * (mi::math::cos(Ay) - mi::math::sin(Ay)) + NON_ZERO;
	float wy = 0.5f + 0.5f * Ax * (mi::math::cos(Ay) - mi::math::sin(Ay)) + NON_ZERO;

	//precalc250
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);
	float NdotV = dot(N, V);
	float HdotX = dot(H, X) / wx;
	float HdotY = dot(H, Y) / wy;
	
	if( NdotL < 0 ) return 0;
	if( NdotV < 0 ) return 0;

	//specular distribution term
	float power = -2 * ( (HdotX*HdotX + HdotY*HdotY) / NonZeroVal(1 + NdotH) );
	float coef = mi::math::rsqrt( NdotL * NdotV );
	float refl = NdotL / (4 * wx * wy);
	float term = coef * refl * mi::math::exp( power );

	return term;
}

//float3 AnisotropicShaderSpecular( TANGENT_VERTEX_OUTPUT IN, float3 worldNormal, float3 lightDir, 
//						   float3 worldEyeDir, IncidentLight light, float SpecularPowerScale )
miColor AnisotropicShaderSpecular( miState *state, sgpu_illum_Anisotropic* params,
								miVector tangent, miVector binormal, miVector sstt,
								miVector worldNormal, miVector lightDir, miVector worldEyeDir,
								miColor lSpecularColor, float SpecularPowerScale)
{	
	miColor specularCombine = Tex2dCombine(state, params->specularMap, params->g_specular, sstt);
	float specularPowerCombine = Tex2dCombine(state, params->specularColorPowerMap, make_color(params->SpecularColorPower), sstt).r;
	float shininessCombine = Tex2dCombine(state, params->shininessMap, make_color(SpecularPowerScale), sstt).r;
	float anisotropy = WardAnisotropicSpecular( worldNormal, lightDir, worldEyeDir, tangent, binormal, params->g_UPower, params->g_VPower );
	float anisotropyCombine = mi::math::saturate(Tex2dCombine(state, params->anisotropyMap, make_color(anisotropy), sstt).r);

	miColor specular = specularPowerCombine * lSpecularColor * specularCombine * pow(anisotropyCombine, max(shininessCombine, 0.001f)) ;
	
	
	return specular;
}

extern "C" DLLEXPORT int sgpu_illum_Anisotropic_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_illum_Anisotropic(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_Anisotropic *paras)
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
	
	miVector worldTangent = state->bump_x_list[0];
	miVector worldBiNormal = state->bump_y_list[0];
	mi_vector_to_world(state,&worldTangent,&worldTangent);
	mi_vector_to_world(state,&worldBiNormal,&worldBiNormal);
	worldTangent = normalize(worldTangent);
	worldBiNormal = normalize(worldBiNormal);
	/*Print("Ani: tangent", worldTangent);
	Print("Ani: binormal", worldBiNormal);*/

	struct sgpu_illum_Anisotropic o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldEyeDir = (state->dir) * -1;
	mi_vector_to_world(state,&worldEyeDir,&worldEyeDir);
	worldEyeDir = normalize(worldEyeDir);
	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

	miColor	transparencyMap_color = Tex2d(state, o_Params.transparencyMap, sstt);
	float g_transparency = o_Params.g_transparency * transparencyMap_color.r;

	miColor sDiffColor = Tex2dCombine( state, o_Params.diffuseMap , o_Params.g_diffuse, sstt );

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
					 o_Params.CubeReflMap,
					 o_Params.reflectFactorMap,
					 o_Params.g_reflectionEnvAngle,
					 o_Params.fresnelBias,
					 o_Params.fresnelPower,
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
					miColor sSpecPower = Tex2dCombine( state, o_Params.shininessMap , make_color(o_Params.g_shininess), sstt );

					//fetch bump normal
					//float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

					// Get color
					miColor ani = AnisotropicShaderSpecular(state, &o_Params, 
						worldTangent, worldBiNormal, sstt,
						worldNormal, lightDir, worldEyeDir, 
						lightColor, o_Params.g_shininess);
					sum += ani;
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

	// transparency blending
	if (g_transparency < 1)
	{
		miColor trans;
		mi_trace_transparent(&trans, state);
        *result = (*result)*g_transparency + trans*(1.0f - g_transparency);
	}

	return(miTRUE);
}