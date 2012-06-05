/******************************************************************************
 * Exports:
 *
 *      sgpu_displacement()
 *
 * Description:
 *      Perform vertex displacement function
 *****************************************************************************/

#include "shader.h"
#include "mi/math.h"

#include "Support.h"

struct sgpu_displacement {
	miTag		dispMap;
	miScalar	dispMapScale;
	miScalar	dispMapBias;
	miScalar	dispMapBlur;
	miVector	dispMapUVScale;
	miVector	dispMapSize;
	//miScalar	objScale;

	// UV transform
    miScalar	u_scale;
    miScalar	v_scale;
    miScalar	u_offset;
    miScalar	v_offset;
	miScalar	uv_rotation;
};

miMatrix WeightMat =
{
	0, 2, 0, 0,
	-1, 0, 1, 0, 
	2,-5, 4,-1,
	-1, 3,-3, 1 
};

float dot(const miColor& a, const miColor& b)
{
	return a.r * b.r + a.g * b.g + a.b * b.b + a.a * b.a;
}

miColor MatMul(const miMatrix& m, const miColor& c)
{
	miColor ret = make_color(0);
	ret.r = WeightMat[0] * c.r + WeightMat[4] * c.g + WeightMat[8] * c.b + WeightMat[12] * c.a;
	ret.g = WeightMat[1] * c.r + WeightMat[5] * c.g + WeightMat[9] * c.b + WeightMat[13] * c.a;
	ret.b = WeightMat[2] * c.r + WeightMat[6] * c.g + WeightMat[10] * c.b + WeightMat[14] * c.a;
	ret.a = WeightMat[3] * c.r + WeightMat[7] * c.g + WeightMat[11] * c.b + WeightMat[15] * c.a;

	return ret;
}

float cubicFilter( float X, miColor C)
{
	miColor t = make_color(1,X,X*X,X*X*X) * 0.5f;
	miColor h = MatMul(WeightMat, t);// = mul( t, WeightMat );
	return dot( C, h);
}

float cubicFilterTan( float X, miColor C)
{
	miColor t = make_color(0,1,2*X,3*X*X) * 0.5f;
	miColor h = MatMul(WeightMat, t);//( t, WeightMat );
	return dot( C, h);
}

miColor Tex2dOffset( miState* i_State, const miTag& i_TextureTag, miVector sstt,
					miVector offset, miVector dim)
{
	miVector uv = make_vector(0); //sstt + (offset / dim);
	uv.x = offset.x / dim.x;
	uv.y = offset.y / dim.y;

	mi_vector_add(&uv, &uv, &sstt);

	miColor color = make_color(0);
	color = Tex2d(i_State,i_TextureTag,uv,false);
	
	return color;
}

miScalar SampleDisplacementBiCubic( miState *state,
								   const miTag& dispMap,
								   const miScalar& dispMapScale,
								   const miScalar& dispMapBias,
								   const miVector& dispMapUVScale,
								   const miVector& dispMapSize,
								   const miVector& UV, 
								   const miScalar& LOD, 
								   miVector& Normal )
{
	miVector Dims = make_vector(dispMapSize.x, dispMapSize.y, 0);
	//float Mips;
	//g_DisplacementMap.GetDimensions( LOD, Dims.x, Dims.y, Mips );

	float MipLevel = LOD-0.5f;	//remove stupid half texel mip level offset

	//Note this aligns the lower mip levels for central sampling,
	//but causes sample rounding errors
	//without the sampling works correctly but lower mip levels shift
	miVector DisUV = UV;//+= -0.5f / Dims;
	DisUV.x += -0.5f / Dims.x;
	DisUV.y += -0.5f / Dims.y;

	//remove half texel offset
	miVector UVDims = DisUV * Dims;
	miVector f = make_vector(mi::math::frac(UVDims.x),
							mi::math::frac(UVDims.y),
							0);  // we want the sub-texel portion

	miMatrix P;
	P[0] = Tex2dOffset(state, dispMap, DisUV, make_vector(-1, -1, 0), Dims).r;//g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(-1, -1)).r;
	P[1] = Tex2dOffset(state, dispMap, DisUV, make_vector(0, -1, 0), Dims).r;
	P[2] = Tex2dOffset(state, dispMap, DisUV, make_vector(1, -1, 0), Dims).r;
	P[3] = Tex2dOffset(state, dispMap, DisUV, make_vector(2, -1, 0), Dims).r;
	P[4] = Tex2dOffset(state, dispMap, DisUV, make_vector(-1, 0, 0), Dims).r;
	P[5] = Tex2dOffset(state, dispMap, DisUV, make_vector(0, 0, 0), Dims).r;
	P[6] = Tex2dOffset(state, dispMap, DisUV, make_vector(1, 0, 0), Dims).r;
	P[7] = Tex2dOffset(state, dispMap, DisUV, make_vector(2, 0, 0), Dims).r;
	P[8] = Tex2dOffset(state, dispMap, DisUV, make_vector(-1, 1, 0), Dims).r;
	P[9] = Tex2dOffset(state, dispMap, DisUV, make_vector(0, 1, 0), Dims).r;
	P[10] = Tex2dOffset(state, dispMap, DisUV, make_vector(1, 1, 0), Dims).r;
	P[11] = Tex2dOffset(state, dispMap, DisUV, make_vector(2, 1, 0), Dims).r;
	P[12] = Tex2dOffset(state, dispMap, DisUV, make_vector(-1, 2, 0), Dims).r;
	P[13] = Tex2dOffset(state, dispMap, DisUV, make_vector(0, 2, 0), Dims).r;
	P[14] = Tex2dOffset(state, dispMap, DisUV, make_vector(1, 2, 0), Dims).r;
	P[15] = Tex2dOffset(state, dispMap, DisUV, make_vector(2, 2, 0), Dims).r;

	miColor Column;

	Column.r = cubicFilter( f.x, make_color(P[0], P[1], P[2], P[3]) );
	Column.g = cubicFilter( f.x, make_color(P[4], P[5], P[6], P[7]) );
	Column.b = cubicFilter( f.x, make_color(P[8], P[9], P[10], P[11]) );
	Column.a = cubicFilter( f.x, make_color(P[12], P[13], P[14], P[15]) );
	float height = cubicFilter( f.y, Column );
	/*float height = 0.0625f * (P[0] + P[1] + P[2] + P[3] + P[4] + P[5] +
								P[6] + P[7] + P[8] + P[9] + P[10] + P[11] + 
								P[12] + P[13] + P[14] + P[15]);*/
	float BiTan = cubicFilterTan( f.y, Column );

	Column.r = cubicFilterTan( f.x, make_color(P[0], P[1], P[2], P[3]) );
	Column.g = cubicFilterTan( f.x, make_color(P[4], P[5], P[6], P[7]) );
	Column.b = cubicFilterTan( f.x, make_color(P[8], P[9], P[10], P[11]) );
	Column.a = cubicFilterTan( f.x, make_color(P[12], P[13], P[14], P[15]) );
	float Tan = cubicFilter( f.y, Column );

//	float UVSize = g_ObjectUVScale.x / g_DisplacementScale;
	float UVSize = abs(dispMapScale*dispMapUVScale.x);
//	float UVSize = abs(g_DisplacementScale*10);

	miVector nx = make_vector( 1, 0, Tan*UVSize );
	miVector ny = make_vector( 0, 1, BiTan*UVSize ); 
	mi_vector_prod(&Normal, &nx, &ny);
	Normal = normalize(Normal);

	//miScalar height = Tex2d(state, dispMap, UV).r;

	return (height + dispMapBias) * dispMapScale;
}

//--------------------------------------------------------------------
// DisplaceVertexNormal()
//--------------------------------------------------------------------
miScalar DisplaceVertexNormal( miState *state,
							  const miTag& dispMap,
							  const miScalar& dispMapScale,
							  const miScalar& dispMapBias,
							  const miScalar& dispMapBlur,
							  const miVector& dispMapUVScale,
							  const miVector& dispMapSize,
							  const miVector& TexCoords,
							  miVector& Normal)
{
	if( dispMap )
	{
		miVector NA, NB;
		miScalar LOD = dispMapBlur * 10.0f;
		float LOD0 = SampleDisplacementBiCubic( state, dispMap, dispMapScale, dispMapBias, dispMapUVScale, dispMapSize, TexCoords, LOD, NA );
		float LOD1 = SampleDisplacementBiCubic( state, dispMap, dispMapScale, dispMapBias, dispMapUVScale, dispMapSize, TexCoords, LOD+1, NB );
		Normal = normalize( lerp( NA, NB, mi::math::frac( LOD )));
		return mi::math::lerp( LOD0, LOD1, mi::math::frac( LOD ));
		
	}
	else return 0.0f;
}

void EvalParams(miState *state,
				const sgpu_displacement* paras,
				sgpu_displacement& o_Params)
{ 
	// shader params
	o_Params.dispMap = *mi_eval_tag(&paras->dispMap);
	o_Params.dispMapScale = *mi_eval_scalar(&paras->dispMapScale);
	o_Params.dispMapBias = *mi_eval_scalar(&paras->dispMapBias);
	o_Params.dispMapBlur = *mi_eval_scalar(&paras->dispMapBlur);
	o_Params.dispMapUVScale = *mi_eval_vector(&paras->dispMapUVScale);
	o_Params.dispMapSize = *mi_eval_vector(&paras->dispMapSize);
	//o_Params.objScale = *mi_eval_scalar(&params->objScale);

	// UV transform
    o_Params.u_scale  = *mi_eval_scalar(&paras->u_scale);
    o_Params.v_scale  = *mi_eval_scalar(&paras->v_scale);
    o_Params.u_offset = *mi_eval_scalar(&paras->u_offset);
    o_Params.v_offset = *mi_eval_scalar(&paras->v_offset);
	o_Params.uv_rotation = *mi_eval_scalar(&paras->uv_rotation);
}

extern "C" DLLEXPORT int sgpu_displacement_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_displacement(
	miScalar		*result,
	miState		*state,
	struct sgpu_displacement *paras)
{
	struct sgpu_displacement o_Params;
	EvalParams(state, paras, o_Params);
	miVector sstt = GetTransformedUVs(state,o_Params.u_scale,o_Params.v_scale,o_Params.u_offset,o_Params.v_offset,o_Params.uv_rotation);
	//Print("Disp uv", sstt);

	miVector pos;
	mi_point_to_world(state, &pos, &state->point);
	//miVector normal;
	//mi_normal_to_world(state, &normal, &state->normal);
	//normal = normalize(normal);
	/*miScalar height = DisplaceVertexNormal(state, 
											o_Params.dispMap,
											o_Params.dispMapScale,
											o_Params.dispMapBias,
											o_Params.dispMapBlur,
											o_Params.dispMapUVScale,
											o_Params.dispMapSize,
											sstt,
											normal);*/

	miScalar height = (Tex2d(state,o_Params.dispMap,sstt,false).r + o_Params.dispMapBias) * o_Params.dispMapScale;
	pos = pos + normalize(state->normal) * height;
	mi_point_from_world(state, &state->point, &pos);
	//state->normal = normal;
	//*result += height * o_Params.objScale;
	return (miTRUE);
}