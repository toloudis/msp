/*****************************************************************************
**	vtxDeltaCompression.hpp
**
**		Working with compression of vertex animation algorithms.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_DELTACOMPRESSION_HPP
#error smdlVertexObject.hpp multiply included
#endif
#define VTX_DELTACOMPRESSION_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct vtxVertexFrame;


//============================================================================
//============================================================================
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
	void GetExtrapolatedDeltaNormals(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas);

	//--------------------------------------------------------------------
	// Assuming o_Frame2 is filled with delta vectors, add in the
	// extrapolation from the two previous frames.
	//--------------------------------------------------------------------
	void ReconstructFromExtrapolatedDeltas(const vtxVertexFrame &i_Frame0,
										   const vtxVertexFrame &i_Frame1,
										   vtxVertexFrame &o_Frame2,
										   bool i_bReconstructNormals = true);

	//--------------------------------------------------------------------
	// Convert the delta 3D vectors into a stream of 16-bit encoded
	//	quantized delta vectors.
	// o_pCodes should be large enough to receive 3 times the number
	//	of deltas (because each X,Y,Z gets a code).
	// The o_QuantLevel and o_MinBounds returned should be given 
	// again when decoding.
	// If all deltas could be encoded within the maximum quantization
	//	then "true" is returned. If false is returned, then some
	//  data would be lost if this encoding would be used. The 
	//	recommendation when returning false is to stop the encoding
	//	and export a full frame of animation data.
	//--------------------------------------------------------------------
	bool EncodeDeltaVectors(const std::vector<maPoint3d> &i_Deltas,
							float i_Tolerance,
							CodeType* o_pCodes,
							maVector3d &o_MinBounds,
							float &o_QuantLevel);

	//--------------------------------------------------------------------
	// Convert the 16-bit encoded quantized delta vectors back into
	// a vector of 3D deltas. The o_Deltas vector should already
	// be the correct size.
	//--------------------------------------------------------------------
	bool DecodeDeltaVectors(const CodeType* i_pCodes,
							float i_QuantLevel,
							const maVector3d& i_MinBounds,
							std::vector<maPoint3d> &o_Deltas);

	//--------------------------------------------------------------------
	// Convert the normal delta 3D vectors into a stream of 16-bit encoded
	//	quantized delta vectors.
	// Each normal will be encoded as best as possible within one 32-bit
	// code that gives two components 15 bits and uses the rest for the
	// sign of the Z component.
	//--------------------------------------------------------------------
	void EncodeNormalVectors(const std::vector<maVector3d> &i_Deltas,
							CodeType* o_pCodes,
							maVector3d &o_MinBounds,
							float &o_QuantLevel);
	void DecodeNormalVectors(const CodeType* i_pCodes,
							float i_QuantLevel,
							const maVector3d& i_MinBounds,
							std::vector<maPoint3d> &o_Deltas);


	//--------------------------------------------------------------------
	// Given two surrounding frames and the coding for the delta vectors,
	// reconstruct the interpolated frame.
	//--------------------------------------------------------------------
	//void DecodeInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
	//					 const vtxVertexFrame &i_Frame2,
	//					 const std::vector<CodeType> &i_Codes,
	//					 const maAxisBox &i_Bounds,
	//					 vtxVertexFrame &o_Frame1);

	//--------------------------------------------------------------------
	// Given two previous frames and the coding for the delta vectors,
	// reconstruct the next frame.
	//--------------------------------------------------------------------
	//void DecodeExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
	//					 const vtxVertexFrame &i_Frame21,
	//					 const std::vector<CodeType> &i_Codes,
	//					 const maAxisBox &i_Bounds,
	//					 vtxVertexFrame &o_Frame2);

}

