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
 *      sgpu_illum_Shadows()
 *
 * History:
 *      20.10.97: initial version
 *	17.11.97: added ambience parameter
 *	27.01.98: added global Shadows capabilities
 *
 * Description:
 *      Perform Shadows with the following reflection model:
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
struct sgpu_illum_Shadows {

	// Shader params
	miScalar	g_transparency;
	miTag		transparencyMap;

	// UV transform
    miScalar	u_scale;
    miScalar	v_scale;
    miScalar	u_offset;
    miScalar	v_offset;
	miScalar	uv_rotation;

	// Normal map
	miTag		normalMap;
	miScalar	normalMapScale;

	int		mode;       /* light mode: 0..2 */
	int		i_light;	/* index of first light */
	int		n_light;	/* number of lights */
	miTag	light[1];	/* list of lights */
};

void EvalParams(miState *state,
				const sgpu_illum_Shadows* paras,
				sgpu_illum_Shadows& o_Params)
{ 
	// Shader params
	o_Params.g_transparency = *mi_eval_scalar(&paras->g_transparency);
	o_Params.transparencyMap = *mi_eval_tag(&paras->transparencyMap);

	// UV transform
    o_Params.u_scale  = *mi_eval_scalar(&paras->u_scale);
    o_Params.v_scale  = *mi_eval_scalar(&paras->v_scale);
    o_Params.u_offset = *mi_eval_scalar(&paras->u_offset);
    o_Params.v_offset = *mi_eval_scalar(&paras->v_offset);
	o_Params.uv_rotation = *mi_eval_scalar(&paras->uv_rotation);

	// Normal map
	o_Params.normalMap = *mi_eval_tag(&paras->normalMap);
	o_Params.normalMapScale = *mi_eval_scalar(&paras->normalMapScale);

}


extern "C" DLLEXPORT int sgpu_illum_Shadows_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_illum_Shadows(
	miColor		*result,
	miState		*state,
	struct sgpu_illum_Shadows *paras)
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

	struct sgpu_illum_Shadows o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);

	miVector worldNormal = GetBumpNormal(state, o_Params.normalMap, o_Params.normalMapScale, sstt);

	float g_transparency = o_Params.g_transparency * Tex2d(state, o_Params.transparencyMap, sstt).r;

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

				float cosine = saturate(dot(lightDir,worldNormal));

				sum += lightColor * cosine;

			}

			samples = iter->get_number_of_samples();

			if (samples) {
				*result += sum / (miScalar)samples;
			}
		}
	}

	*result = make_color(1) - *result;

	// transparency blending
	if (g_transparency < 1)
	{
		miColor trans;
		mi_trace_transparent(&trans, state);
		*result = (*result)*g_transparency + trans*(1.0f - g_transparency);
	}

	return(miTRUE);
}


