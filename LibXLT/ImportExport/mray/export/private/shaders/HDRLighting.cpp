 /******************************************************************************
 * Created:	20.10.97
 * Module:	baseshader
 * Purpose:	base shaders for Phenomenon writers
 *
 * Exports:
 *
 *      sgpu_illum_Lambert()
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

//--------------------------------------------------------------------
// RGB2Yxy()
//--------------------------------------------------------------------
miVector RGB2Yxy(miColor colorrgb)
{
	miMatrix RGB2XYZ = {
						0.5141364f, 0.3238786f,  0.16036376f, 0.0f,
						0.265068f,  0.67023428f, 0.06409157f, 0.0f,
						0.0241188f, 0.1228178f,  0.84442666f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f};

	miVector vectorrgb = make_vector( colorrgb.r, colorrgb.g, colorrgb.b );
	miVector XYZ;
	mi_point_transform(&XYZ, &vectorrgb, RGB2XYZ);	

	miVector Yxy;
	Yxy.x = XYZ.y;
	Yxy.y = XYZ.x / (XYZ.x + XYZ.y + XYZ.z);
	Yxy.z = XYZ.y / (XYZ.x + XYZ.y + XYZ.z);

	return Yxy;
}

//--------------------------------------------------------------------
// RGB2Yxy()
//--------------------------------------------------------------------
miColor Yxy2RGB(miVector colorYxy)
{
	miMatrix XYZ2RGB = {
						2.5651f,-1.1665f,-0.3986f, 0.0f,
						-1.0217f, 1.9777f, 0.0439f, 0.0f,
						0.0753f, -0.2543f, 1.1892f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f};

	miVector XYZ;
	// Yxy -> XYZ conversion
	XYZ.x = colorYxy.x * colorYxy.y / colorYxy.z;
	XYZ.y = colorYxy.x;
	XYZ.z = colorYxy.x * (1 - colorYxy.y - colorYxy.z) / colorYxy.z;

	miVector retVec;
	mi_point_transform(&retVec, &XYZ, XYZ2RGB);

	return make_color(retVec.x,retVec.y,retVec.z,1);
}

//--------------------------------------------------------------------
// PS_Reinhard02()
//--------------------------------------------------------------------
miColor PS_Reinhard02( miColor hdrColorSample, float avgLuminance, 
					   float exposure, float whitePoint )
{
	// map pixel to luminance space
	miVector Yxy = RGB2Yxy(hdrColorSample);

	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	float Lp = Yxy.x * exposure / avgLuminance;     

	// (Ld) Scale all luminance within a displayable range of 0 to 1
	Yxy.x = (Lp * (1.0f + Lp/(whitePoint * whitePoint)))/(1.0f + Lp);

	miColor out = Yxy2RGB(Yxy);
	out.a = clamp( out.a, 0.0f , 1.0f );

	return out;
}


struct sgpu_HDRLighting {
	miBoolean   g_bEnableTonemap;
	miBoolean   g_bReflOnly;
	miBoolean   g_bGIOnly;
	miScalar	g_fixedLuminance;
	miScalar	g_fMiddleGray;
	miScalar	g_fWhiteCutoff;
};

void EvalParams(miState *state,
				const sgpu_HDRLighting* paras,
				sgpu_HDRLighting& o_Params)
{ 
	// shader params
	o_Params.g_bEnableTonemap = *mi_eval_boolean(&paras->g_bEnableTonemap);
	o_Params.g_bReflOnly = *mi_eval_boolean(&paras->g_bReflOnly);
	o_Params.g_bGIOnly = *mi_eval_boolean(&paras->g_bGIOnly);
	o_Params.g_fixedLuminance = *mi_eval_scalar(&paras->g_fixedLuminance);
	o_Params.g_fMiddleGray = *mi_eval_scalar(&paras->g_fMiddleGray);
	o_Params.g_fWhiteCutoff = *mi_eval_scalar(&paras->g_fWhiteCutoff);
}

extern "C" DLLEXPORT int sgpu_HDRLighting_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_HDRLighting(
    void                 *result,
    miState              *state,
    struct sgpu_HDRLighting *paras)
{
	struct sgpu_HDRLighting o_Params;
	EvalParams(state, paras, o_Params);

    int         x, y;
    miColor     renderedColor;
    miImg_image *fb_color;

	miUint bufferIdx = miRC_IMAGE_RGBA;

	if ( o_Params.g_bReflOnly || o_Params.g_bGIOnly )
	{
		bufferIdx = miRC_IMAGE_USER;
	}

	fb_color = mi_output_image_open(state, bufferIdx);

	for (y=0; y < state->camera->y_resolution; y++) {
		if (mi_par_aborted()) break;
		for (x=0; x<state->camera->x_resolution; x++) {
			mi_img_get_color(fb_color, &renderedColor, x, y);

			miColor finalColor = renderedColor;

			if ( o_Params.g_bEnableTonemap )
			{
				finalColor = PS_Reinhard02( renderedColor, 
					o_Params.g_fixedLuminance + 0.0001f, 
					o_Params.g_fMiddleGray, 
					o_Params.g_fWhiteCutoff );
			}

			mi_img_put_color(fb_color, &finalColor, x, y);
		}
	}
    mi_output_image_close(state, bufferIdx);


    return(miTRUE);
}

