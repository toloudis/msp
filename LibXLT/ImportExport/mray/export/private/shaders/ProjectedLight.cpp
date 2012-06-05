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
 *      sgpu_light_ProjectedLight()
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

struct sgpu_light_ProjectedLight {
	miColor		color;		/* color of light source */
	miVector	Pos;
	miScalar	PosW;
	miBoolean	shadow;		/* light casts shadows */
	miScalar	factor;		/* makes opaque objects transparent */
	miColor		ShadowColor;
	miScalar	ShadowIntensity;
	miScalar	FalloffX;
	miScalar	FalloffY;
	miScalar	FalloffZ;
	miScalar	FalloffW;
	miScalar	FalloffStart;	/* if atten, distance range */
	miScalar	Scale;	/* if atten, distance range */
	miScalar	Range;	/* if atten, distance range */
	miScalar	Aspect;	/* if atten, distance range */
	miScalar	OuterAngle;		/* outer solid cone */
	miScalar	InnerAngle;		/* inner solid cone */
	miScalar	DepthBias;
	miMatrix	CameraMatrix;
	miMatrix	ProjMatrix;
	miMatrix	CamProjMatrix;
	miTag		Texture;
	miBoolean	AffectsDiffuse;
	miBoolean	AffectsSpecular;

	miBoolean	bDirectional;
	miBoolean	bPureDirectional;
	miBoolean	bConeLighting;

	miBoolean	bShadowsOnly;
	miBoolean	bEnabled;

	miScalar	bbox_min_x;
	miScalar	bbox_min_y;
	miScalar	bbox_min_z;
	miScalar	bbox_max_x;
	miScalar	bbox_max_y;
	miScalar	bbox_max_z;
};

void EvalParams(miState *state,
				const sgpu_light_ProjectedLight* paras,
				sgpu_light_ProjectedLight& o_Params)
{
	o_Params.color = *mi_eval_color(&paras->color);		/* color of light source */
	o_Params.Pos = *mi_eval_vector(&paras->Pos);
	o_Params.shadow = *mi_eval_boolean(&paras->shadow);		/* light casts shadows */
	o_Params.factor = *mi_eval_scalar(&paras->factor);		/* makes opaque objects transparent */
	o_Params.ShadowColor = *mi_eval_color(&paras->ShadowColor);
	o_Params.ShadowIntensity = *mi_eval_scalar(&paras->ShadowIntensity);
	o_Params.FalloffX = *mi_eval_scalar(&paras->FalloffX);
	o_Params.FalloffY = *mi_eval_scalar(&paras->FalloffY);
	o_Params.FalloffZ = *mi_eval_scalar(&paras->FalloffZ);
	o_Params.FalloffW = *mi_eval_scalar(&paras->FalloffW);
	o_Params.FalloffStart = *mi_eval_scalar(&paras->FalloffStart);	/* if atten, distance range */
	o_Params.Scale = *mi_eval_scalar(&paras->Scale);
	o_Params.Range = *mi_eval_scalar(&paras->Range);	/* if atten, distance range */
	o_Params.Aspect = *mi_eval_scalar(&paras->Aspect);
	o_Params.OuterAngle = *mi_eval_scalar(&paras->OuterAngle);		/* outer solid cone */
	o_Params.InnerAngle = *mi_eval_scalar(&paras->InnerAngle);		/* inner solid cone */
	mi_matrix_copy(o_Params.CameraMatrix, mi_eval_transform(&paras->CameraMatrix));
	mi_matrix_copy(o_Params.ProjMatrix, mi_eval_transform(&paras->ProjMatrix));
	mi_matrix_copy(o_Params.CamProjMatrix, mi_eval_transform(&paras->CamProjMatrix));
	o_Params.Texture = *mi_eval_tag(&paras->Texture);
	o_Params.bDirectional = *mi_eval_boolean(&paras->bDirectional);
	o_Params.bPureDirectional = *mi_eval_boolean(&paras->bPureDirectional);
	o_Params.bConeLighting = *mi_eval_boolean(&paras->bConeLighting);
	o_Params.bShadowsOnly = *mi_eval_boolean(&paras->bShadowsOnly);
	o_Params.bEnabled = *mi_eval_boolean(&paras->bEnabled);

	o_Params.bbox_min_x = *mi_eval_scalar(&paras->bbox_min_x);
	o_Params.bbox_min_y = *mi_eval_scalar(&paras->bbox_min_y);
	o_Params.bbox_min_z = *mi_eval_scalar(&paras->bbox_min_z);

	o_Params.bbox_max_x = *mi_eval_scalar(&paras->bbox_max_x);
	o_Params.bbox_max_y = *mi_eval_scalar(&paras->bbox_max_y);
	o_Params.bbox_max_z = *mi_eval_scalar(&paras->bbox_max_z);
	o_Params.AffectsDiffuse = *mi_eval_boolean(&paras->AffectsDiffuse);	
	o_Params.AffectsSpecular = *mi_eval_boolean(&paras->AffectsSpecular);
	o_Params.DepthBias = *mi_eval_scalar(&paras->DepthBias);
}

//--------------------------------------------------------------------
// clipSuperellipse()
//--------------------------------------------------------------------
float clipSuperellipse(const miVector& Q, const miVector& Pc,
					   const float& innerAngle, const float& outerAngle, 
					   const float& scale, const float& aspect, const float& range,
					   const bool& i_bConeLighting, const bool& i_bDirectional,
					   const float& bbox_min_x, const float& bbox_min_y, const float& bbox_min_z,
					   const float& bbox_max_x, const float& bbox_max_y, const float& bbox_max_z 
				 )
{
	float result = 0;
	float x = abs(Q.x);
	float y = abs(Q.y);
	float inner = (innerAngle/2) * AngleToRad;
	float outer = (outerAngle/2) * AngleToRad;

    if ( !i_bDirectional && !i_bConeLighting ) 
	{  	
		if ( Pc.z > scale && Pc.z < scale+range )
		{
			float A = tan( outer );
			float B = tan( outer ) * (1/(aspect+NON_ZERO));
			result = 1 - (1-step(A,x)) * (1-step(B,y));
		} 
		else result = 1.0f;
	} 

	else if ( !i_bDirectional && i_bConeLighting ) 
	{		
		if ( Pc.z > scale && Pc.z < scale+range )
		{
			float a = tan(inner);
			float b = tan(inner) * (1/(aspect+NON_ZERO));
			float A = tan(outer);
			float B = tan(outer) * (1/(aspect+NON_ZERO));
			float q = a*b/sqrt(b*b*x*x + a*a*y*y);
			float r = A*B/sqrt(B*B*x*x + A*A*y*y);
			result = smoothstep(q, r, 1);
		}
		else result = 1.0f;
    }

	else if ( i_bDirectional && !i_bConeLighting ) 
	{
		miVector shiftedPt = Pc;
		shiftedPt.z = shiftedPt.z - scale;
		result = 1.0f - ContainsPoint(shiftedPt,
								   bbox_min_x,bbox_min_y,bbox_min_z,
								   bbox_max_x,bbox_max_y,bbox_max_z);
	}

	else if ( i_bDirectional && i_bConeLighting ) 
	{	
		float currRadius = sqrt( Pc.x*Pc.x + Pc.y*Pc.y*aspect*aspect );
		result = (currRadius > scale/2) ? 1.0f : 0.0f;
	}

    return result;
}

miColor CalcLight( miState * state , const miVector& Pc, const miVector& Pcp, sgpu_light_ProjectedLight o_Params, float d)
{

	bool unoccluded = true;
	miColor lcol = o_Params.color;
	if ( o_Params.bShadowsOnly )
	{
		lcol = make_color(1);
	}

	miColor c_out = make_color(0);

	// Attenuation formula
	float atten = attenuation((float)(state->dist), o_Params.FalloffX, o_Params.FalloffY, o_Params.FalloffZ, o_Params.FalloffW, o_Params.FalloffStart );
	
	if ( !o_Params.bPureDirectional )
	{		
		// Clip cone-shaped light into rectangle/ellipse
		atten *= 1 - clipSuperellipse(Pc/Pc.z, Pc, o_Params.InnerAngle, o_Params.OuterAngle, 
									  o_Params.Scale, o_Params.Aspect, o_Params.Range,
									  o_Params.bConeLighting, o_Params.bDirectional, 
									  o_Params.bbox_min_x, o_Params.bbox_min_y, o_Params.bbox_min_z, 
									  o_Params.bbox_max_x, o_Params.bbox_max_y, o_Params.bbox_max_z  );
	}

	// Shadows
	miColor shadowCoeff = make_color(1);	
	if (o_Params.shadow) {			
		d = o_Params.factor;
		if (d < 1) {
			miColor filter = make_color(1);
			
			// modify sampling point according to bias setting
			miVector oriPoint = state->point;
			miVector biasPoint = state->point;
			miMatrix invProjMat;
			mi_matrix_invert(invProjMat, o_Params.CamProjMatrix);
			// modify it in projected space
			mi_point_transform( &biasPoint , &biasPoint , o_Params.CamProjMatrix );
			biasPoint.z -= o_Params.DepthBias;
			mi_point_transform( &biasPoint, &biasPoint, invProjMat);
			state->point = biasPoint;

			unoccluded = mi_trace_shadow(&filter,state);
			// change back the point
			state->point = oriPoint;
			// opaque
			if (!unoccluded || BLACK(filter)) 
			{
				shadowCoeff = make_color(d);
			} 

			// transparent
			else 
			{	
				miScalar omf = 1 - d;
				shadowCoeff = make_color(d) + omf * filter;
			}
		}
	}
	if ( o_Params.bShadowsOnly )
	{
		c_out = lcol * atten * ( 1.0f - (unoccluded?1.0f:0.0f) );
	}
	else
	{
		shadowCoeff = lerp(make_color(1-o_Params.ShadowIntensity), make_color(1), shadowCoeff);

		// Apply gobo
		miVector sstt = make_vector( 1 - ( ( Pcp.x + 1 ) / 2 ) , ( Pcp.y + 1 ) / 2 , 1 );
		miColor tex_col = Tex2d(state, o_Params.Texture, sstt);

		// multiply texture color with shadow color
		miColor modifier = o_Params.ShadowColor * atten * tex_col;
		c_out = lerp( modifier, atten * tex_col * lcol, shadowCoeff);
	}

	return c_out;
}

extern "C" DLLEXPORT int sgpu_light_ProjectedLight_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_light_ProjectedLight(
	register miColor	*result,
	register miState	*state,
	register struct sgpu_light_ProjectedLight *paras)
{
	miScalar		d;
	miScalar		outerAngleSpread;
	miVector		ldir;
	miVector		dir;
	miTag			ltag;

	if (state->type != miRAY_LIGHT)			/* visible area light*/
		return(miTRUE);
	
	// angle atten
	ltag = ((miInstance *)mi_db_access(state->light_instance))->item;
	mi_db_unpin(state->light_instance);
	mi_query(miQ_LIGHT_DIRECTION, state, ltag, &ldir);
	mi_vector_to_light(state, &dir, &state->dir);
	d = dot(dir, ldir);

	struct sgpu_light_ProjectedLight o_Params;
	EvalParams(state, paras, o_Params);

	if ( !o_Params.bEnabled )
	{
		*result = make_color(0);
		return(miFALSE);
	}
	
	miVector Pc = state->point;
	mi_point_transform( &Pc , &Pc , o_Params.CameraMatrix );

	miVector Pcp = state->point;
	mi_point_transform( &Pcp , &Pcp , o_Params.CamProjMatrix );
	*result = CalcLight( state, Pc, Pcp , o_Params, d );

	sgpuLightData ltData = {o_Params.AffectsDiffuse, o_Params.AffectsSpecular};
	mi_shaderstate_set( state, SGPU_LIGHT_DATA_NAME, &ltData, sizeof(sgpuLightData), miSS_LIFETIME_EYERAY );

	// some light is incident
	return(miTRUE);
}