/*****************************************************************************
**  vtxDeltaCompression.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "vtxDeltaCompression.hpp"

#include "Graphics/vtx/vtxVertexFrame.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Core/dbg/dbgMsg.hpp"


namespace vtxDeltaCompression
{
	namespace
	{
		const float c_ErrorThreshold = 0.01f;
		const float c_MaxQuant = 65535;
		//const float c_MaxQuant = 255;
	}

	//--------------------------------------------------------------------
	// Get delta vectors as the difference between the middle frame and
	//	its two surrounding frames.
	//--------------------------------------------------------------------
	void GetInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas)
	{
		int num_pts = i_Frame0.m_Positions.size();
		o_Deltas.resize(num_pts);

		//maAxisBox bounds;
		for (int i=0; i<num_pts; ++i)
		{
			o_Deltas[i] = i_Frame1.m_Positions[i] - (i_Frame2.m_Positions[i] + i_Frame0.m_Positions[i]) * 0.5f;

			//bounds.Union(o_Deltas[i]);
		}

		//DBG_LOG("Bounds: " << bounds.GetRadius());
	}

	//--------------------------------------------------------------------
	// Get delta vectors as the difference between a middle frame and
	//	two references frames. 
	// i_Alpha sets how far along the segment the middle frame is
	//--------------------------------------------------------------------
	void GetInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 float i_Alpha,
						 std::vector<maPoint3d> &o_Deltas)
	{
		int num_pts = i_Frame0.m_Positions.size();
		o_Deltas.resize(num_pts);

		for (int i=0; i<num_pts; ++i)
		{
			o_Deltas[i] = i_Frame1.m_Positions[i] - (i_Frame2.m_Positions[i]*i_Alpha + i_Frame0.m_Positions[i]*(1-i_Alpha));
		}
	}

	//--------------------------------------------------------------------
	// Get delta vectors as difference of the second frame from an 
	// extrapolating predicotr using the first two frames
	//--------------------------------------------------------------------
	void GetExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas)
	{
		int num_pts = i_Frame0.m_Positions.size();
		o_Deltas.resize(num_pts);

		for (int i=0; i<num_pts; ++i)
		{
			o_Deltas[i] = i_Frame2.m_Positions[i] - ( 2*i_Frame1.m_Positions[i] - i_Frame0.m_Positions[i]);
		}
	}
	
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
							maAxisBox &o_Bounds)
	{
		bool bWithinQuantLevel = true;
		int num_pts = i_Deltas.size();
		o_Codes.resize(3*num_pts);

		o_Bounds = maAxisBox();
		for (int i=0; i<num_pts; ++i)
			o_Bounds.Union(i_Deltas[i]);

		// Encode into 16-bit quantization within the bounds we found
		maPoint3d min_corner = o_Bounds.GetBoxPoint(7);
		maVector3d diff = o_Bounds.GetBoxPoint(0) - min_corner;

		float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
		float quant = max_diff / c_ErrorThreshold;
		if (quant > c_MaxQuant)
		{
			DBG_WARNING("Big Quant: " << quant);
			quant = c_MaxQuant;
			bWithinQuantLevel = false;
		}
		
		CodeType *ptr = &o_Codes[0];
		maVector3d normalized;
		for (int i=0; i<num_pts; ++i)
		{
			normalized = i_Deltas[i] - min_corner;

			if (diff[0] >=0)
				(*ptr++) = (CodeType)(quant * normalized[0] / diff[0]);
			else 
				(*ptr++) = 0;

			if (diff[1] >=0)
				(*ptr++) = (CodeType)(quant * normalized[1] / diff[1]);
			else 
				(*ptr++) = 0;

			if (diff[2] >=0)
				(*ptr++) = (CodeType)(quant * normalized[2] / diff[2]);
			else 
				(*ptr++) = 0;
		}

		return bWithinQuantLevel;
	}


	//--------------------------------------------------------------------
	// Given two surrounding frames and the coding for the delta vectors,
	// reconstruct the interpolated frame.
	//--------------------------------------------------------------------
	void DecodeInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame2,
						 const std::vector<CodeType> &i_Codes,
						 const maAxisBox &i_Bounds,
						 vtxVertexFrame &o_Frame1)
	{
		int num_pts = i_Frame0.m_Positions.size();
		o_Frame1.m_Positions.resize(num_pts);

		maPoint3d min_corner = i_Bounds.GetBoxPoint(7);
		maVector3d diff = i_Bounds.GetBoxPoint(0) - min_corner;

		float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
		float quant = max_diff / c_ErrorThreshold;
		if (quant > c_MaxQuant) 
			quant = c_MaxQuant;

		const CodeType *ptr = &i_Codes[0];
		maVector3d delta, normalized;
		for (int i=0; i<num_pts; ++i)
		{			
			if (quant > 0)
			{
				normalized[0] = (*ptr++) * diff[0] / quant;
				normalized[1] = (*ptr++) * diff[1] / quant;
				normalized[2] = (*ptr++) * diff[2] / quant;
				delta = normalized + min_corner;
			}
			else
				delta = min_corner;

			o_Frame1.m_Positions[i] = delta + (i_Frame2.m_Positions[i] + i_Frame0.m_Positions[i]) * 0.5f;
		}

		//DBG_LOG("Bounds: " << bounds.GetRadius());
	}

	//--------------------------------------------------------------------
	// Given two previous frames and the coding for the delta vectors,
	// reconstruct the next frame.
	//--------------------------------------------------------------------
	void DecodeExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const std::vector<CodeType> &i_Codes,
						 const maAxisBox &i_Bounds,
						 vtxVertexFrame &o_Frame2)
	{
		int num_pts = i_Frame0.m_Positions.size();
		o_Frame2.m_Positions.resize(num_pts);

		maPoint3d min_corner = i_Bounds.GetBoxPoint(7);
		maVector3d diff = i_Bounds.GetBoxPoint(0) - min_corner;

		float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
		float quant = max_diff / c_ErrorThreshold;
		if (quant > c_MaxQuant) 
			quant = c_MaxQuant;

		const CodeType *ptr = &i_Codes[0];
		maVector3d delta, normalized;
		for (int i=0; i<num_pts; ++i)
		{			
			if (quant > 0)
			{	
				normalized[0] = (*ptr++) * diff[0] / quant;
				normalized[1] = (*ptr++) * diff[1] / quant;
				normalized[2] = (*ptr++) * diff[2] / quant;
				delta = normalized + min_corner;
			}
			else
				delta = min_corner;

			o_Frame2.m_Positions[i] = delta + ( 2*i_Frame1.m_Positions[i] - i_Frame0.m_Positions[i]);
		}

		//DBG_LOG("Bounds: " << bounds.GetRadius());
	}
}