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
#include "geoshader.h"
#include "mi_shader_if.h"
#include "mi/math.h"

#include "Support.h"

#define EPS	 1e-4
#define BLACK(C) ((C).r==0 && (C).g==0 && (C).b==0)

/*
 * Spot light source. Must have an origin and direction in the input file.
 * Takes a variable number of parameters. The first is a boolean which says
 * whether the light casts a shadow or not. The second is the shadow factor.
 */

struct sgpu_light_PointLight {
	miColor		color;		/* color of light source */
	miVector	Pos;
	miScalar	FalloffX;
	miScalar	FalloffY;
	miScalar	FalloffZ;
	miScalar	FalloffW;
	miScalar	FalloffStart;	/* if atten, distance range */
	miBoolean	bShadowsOnly;
	miBoolean	bEnabled;
	miBoolean	AffectsDiffuse;
	miBoolean	AffectsSpecular;
};

void EvalParams(miState *state,
				const sgpu_light_PointLight* paras,
				sgpu_light_PointLight& o_Params)
{
	o_Params.color = *mi_eval_color(&paras->color);		/* color of light source */
	o_Params.Pos = *mi_eval_vector(&paras->Pos);		/* light casts shadows */
	o_Params.FalloffX = *mi_eval_scalar(&paras->FalloffX);
	o_Params.FalloffY = *mi_eval_scalar(&paras->FalloffY);
	o_Params.FalloffZ = *mi_eval_scalar(&paras->FalloffZ);
	o_Params.FalloffW = *mi_eval_scalar(&paras->FalloffW);
	o_Params.FalloffStart = *mi_eval_scalar(&paras->FalloffStart);	/* if atten, distance range */
	o_Params.bShadowsOnly = *mi_eval_scalar(&paras->bShadowsOnly);
	o_Params.bEnabled = *mi_eval_boolean(&paras->bEnabled);
	o_Params.AffectsDiffuse = *mi_eval_boolean(&paras->AffectsDiffuse);	
	o_Params.AffectsSpecular = *mi_eval_boolean(&paras->AffectsSpecular);
}
float attenuation(float d,		// Position of vertex in world coords
				  struct sgpu_light_PointLight* light);

extern "C" DLLEXPORT int sgpu_light_PointLight_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_light_PointLight(
	register miColor	*result,
	register miState	*state,
	register struct sgpu_light_PointLight *paras)
{
	struct sgpu_light_PointLight o_Params;
	EvalParams(state, paras, o_Params);

	if ( !o_Params.bEnabled )
	{
		*result = make_color(0);
		return(miFALSE);
	}

	*result = o_Params.color;
	if (state->type != miRAY_LIGHT)			/* visible area light*/
		return(miTRUE);

	/* dist atten*/
	float atten = attenuation((float)(state->dist), o_Params.FalloffX, o_Params.FalloffY, o_Params.FalloffZ, o_Params.FalloffW, o_Params.FalloffStart );

	// multiply texture color with shadow color
	*result = atten * o_Params.color;

	sgpuLightData ltData = {o_Params.AffectsDiffuse, o_Params.AffectsSpecular};
	mi_shaderstate_set( state, SGPU_LIGHT_DATA_NAME, &ltData, sizeof(sgpuLightData), miSS_LIFETIME_EYERAY );

	// some light is incident
	return(miTRUE);
}

