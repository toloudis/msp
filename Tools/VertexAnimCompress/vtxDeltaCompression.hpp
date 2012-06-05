/*****************************************************************************
**  vtxDeltaCompression.hpp
**
**      Working with compression of vertex animation algorithms.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_DELTACOMPRESSION_HPP
#error smdlVertexObject.hpp multiply included
#endif
#define VTX_DELTACOMPRESSION_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 

#include <vector>

struct vtxVertexFrame;

namespace vtxDeltaCompression
{
	typedef envType::UInt16 CodeType;

	//--------------------------------------------------------------------
	// Get delta vectors as the difference between the middle frame and
	//	its two surrounding frames.
	//--------------------------------------------------------------------
	void GetInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas);

	//--------------------------------------------------------------------
	// Get delta vectors as the difference between a middle frame and
	//	two references frames. 
	// i_Alpha sets how far along the segment the middle frame is
	//--------------------------------------------------------------------
	void GetInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 float i_Alpha,
						 std::vector<maPoint3d> &o_Deltas);

	//--------------------------------------------------------------------
	// Get delta vectors as difference of the second frame from an 
	// extrapolating predicotr using the first two frames
	//--------------------------------------------------------------------
	void GetExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas);

	//--------------------------------------------------------------------
	// Convert the delta 3D vectors into a stream of 16-bit encoded
	//	quantized delta vectors.
	// If all deltas could be encoded within the maximum quantization
	//	then "true" is returned. If false is returned, then some
	//  data would be lost if this encoding would be used. The 
	//	recommendation when returning false is to stop the encoding
	//	and export a full frame of animation data.
	//--------------------------------------------------------------------
	bool EncodeDeltaVectors(const std::vector<maPoint3d> &i_Deltas,
							std::vector<CodeType> &o_Codes,
							maAxisBox &o_Bounds);

	//--------------------------------------------------------------------
	// Given two surrounding frames and the coding for the delta vectors,
	// reconstruct the interpolated frame.
	//--------------------------------------------------------------------
	void DecodeInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame2,
						 const std::vector<CodeType> &i_Codes,
						 const maAxisBox &i_Bounds,
						 vtxVertexFrame &o_Frame1);

	//--------------------------------------------------------------------
	// Given two previous frames and the coding for the delta vectors,
	// reconstruct the next frame.
	//--------------------------------------------------------------------
	void DecodeExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame21,
						 const std::vector<CodeType> &i_Codes,
						 const maAxisBox &i_Bounds,
						 vtxVertexFrame &o_Frame2);

}