/*****************************************************************************
**vtxDeltaCompression.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/private/vtxDeltaCompression.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Graphics/vtx/vtxVertexFrame.hpp"


//============================================================================
//============================================================================
namespace vtxDeltaCompression
{
	//============================================================================
	//============================================================================
	namespace
	{
		//const float c_ErrorThreshold = 0.01f;
		const float c_MaxQuant = 65535;				// 16-bits
		//const float c_MaxQuant = 255;
		const float c_MaxNormalQuant = 32767;		// 15-bits
		const envType::UInt16 c_SignBit = 0x8000;	// Use 16th bit for sign of normal's Z component
		const envType::UInt16 c_SignMask = 0x7FFF;	
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
	//--------------------------------------------------------------------
	void GetExtrapolatedDeltaNormals(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 const vtxVertexFrame &i_Frame2,
						 std::vector<maPoint3d> &o_Deltas)
	{
		int num_pts = i_Frame0.m_Normals.size();
		o_Deltas.resize(num_pts);

		float z_comp = 0;
		for (int i=0; i<num_pts; ++i)
		{
			o_Deltas[i].m_X = i_Frame2.m_Normals[i].m_X - ( 2*i_Frame1.m_Normals[i].m_X - i_Frame0.m_Normals[i].m_X);
			o_Deltas[i].m_Y = i_Frame2.m_Normals[i].m_Y - ( 2*i_Frame1.m_Normals[i].m_Y - i_Frame0.m_Normals[i].m_Y);

			// Convert the z component to just the sign value for the
			// z component. It's length can be computed from the X and Y 
			// components when unit length
			z_comp = i_Frame2.m_Normals[i].m_Z;
			o_Deltas[i].m_Z = (z_comp > 0) ? 1.0f : ((z_comp < 0) ? -1.0f : 0.0f);
		}
	}

	//--------------------------------------------------------------------
	// Assuming o_Frame2 is filled with delta vectors, add in the
	// extrapolation from the two previous frames.
	//--------------------------------------------------------------------
	void ReconstructFromExtrapolatedDeltas(const vtxVertexFrame &i_Frame0,
						 const vtxVertexFrame &i_Frame1,
						 vtxVertexFrame &o_Frame2,
						 bool i_bReconstructNormals)
	{
		int num_verts = i_Frame0.m_Positions.size();
		int num_normals = i_Frame0.m_Normals.size();

		for (int v=0; v<num_verts; v++)
			o_Frame2.m_Positions[v] += ( 2*i_Frame1.m_Positions[v] - i_Frame0.m_Positions[v] );

		if (i_bReconstructNormals && (num_normals > 0))
		{
			for (int n=0; n<num_normals; n++)
			{
				// Just extrapolate the X and Y components, the Z is only a sign value
				o_Frame2.m_Normals[n].m_X += ( 2*i_Frame1.m_Normals[n].m_X - i_Frame0.m_Normals[n].m_X );
				o_Frame2.m_Normals[n].m_Y += ( 2*i_Frame1.m_Normals[n].m_Y - i_Frame0.m_Normals[n].m_Y );
			}
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
							float i_Tolerance,
							CodeType* o_pCodes,
							maVector3d &o_MinBounds,
							float &o_QuantLevel)
	{
		bool bWithinQuantLevel = true;
		const int num_pts = i_Deltas.size();

		maAxisBox bounds;
		for (int i=0; i<num_pts; ++i)
			bounds.Union(i_Deltas[i]);

		// Encode into 16-bit quantization within the bounds we found
		maPoint3d min_corner = bounds.GetBoxPoint(7);
		o_MinBounds = min_corner;
		maVector3d diff = bounds.GetBoxPoint(0) - min_corner;

		float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
		DBG_ASSERT(i_Tolerance >= 0, "Tolerance must be larger than zero");
		float quant = max_diff / c_MaxQuant;
		if (quant > i_Tolerance)
		{
			// Giving special meaning to i_Tolerance==0, which will mean
			// always pack deltas into 16-bits as best as possible.
			// this will not handle large movements well
			if (i_Tolerance > 0)
			{
				//DBG_WARNING("Big Quant: " << quant);
				bWithinQuantLevel = false;
			}
		}
		else
		{
			// Not sure about this... If we let the quant stay smaller,
			// then our 16 bit encoding will be more accurate, but we would get less compression.
			quant = i_Tolerance;
		}
		o_QuantLevel = quant;
		
		CodeType *ptr = o_pCodes;
		maVector3d normalized;
		const float half_quant = quant * 0.5f;
		maVector3d offset(half_quant, half_quant, half_quant); // offset half quant level for better rounding
		for (int i=0; i<num_pts; ++i)
		{
			normalized = offset + i_Deltas[i] - min_corner;

			if (quant > 0)
			{
				(*ptr++) = (CodeType)(normalized[0] / quant);
				(*ptr++) = (CodeType)(normalized[1] / quant);
				(*ptr++) = (CodeType)(normalized[2] / quant);
			}
			else
			{
				(*ptr++) = 0;
				(*ptr++) = 0;
				(*ptr++) = 0;
			}
		}

		return bWithinQuantLevel;
	}

	//--------------------------------------------------------------------
	// Convert the 16-bit encoded quantized delta vectors back into
	// a vector of 3D deltas. The o_Deltas vector should already
	// be the correct size.
	//--------------------------------------------------------------------
	bool DecodeDeltaVectors(const CodeType* i_pCodes,
							float i_QuantLevel,
							const maVector3d& i_MinBounds,
							std::vector<maPoint3d> &o_Deltas)
	{
		const int num_pts = o_Deltas.size();

		const CodeType *ptr = i_pCodes;
		maVector3d delta, normalized;
		for (int i=0; i<num_pts; ++i)
		{			
			if (i_QuantLevel > 0)
			{
				normalized[0] = (*ptr++) * i_QuantLevel;
				normalized[1] = (*ptr++) * i_QuantLevel;
				normalized[2] = (*ptr++) * i_QuantLevel;
				o_Deltas[i] = normalized + i_MinBounds;
			}
			else
				o_Deltas[i] = i_MinBounds;
		}
		return true;
	}

	//--------------------------------------------------------------------
	// Convert the normal delta 3D vectors into a stream of 16-bit encoded
	//	quantized delta vectors.
	// The normals will be encoded as best as possible within one 32-bit
	// code that gives two components 15 bits and uses the rest for the
	// sign of the Z component
	//--------------------------------------------------------------------
	void EncodeNormalVectors(const std::vector<maVector3d> &i_Deltas,
							CodeType* o_pCodes,
							maVector3d &o_MinBounds,
							float &o_QuantLevel)
	{
		const int num_pts = i_Deltas.size();

		maAxisBox bounds;
		for (int i=0; i<num_pts; ++i)
			bounds.Union(i_Deltas[i]);

		// Encode into 16-bit quantization within the bounds we found
		maPoint3d min_corner = bounds.GetBoxPoint(7);
		o_MinBounds = min_corner;
		maVector3d diff = bounds.GetBoxPoint(0) - min_corner;

		// Just X and Y components for bounds
		float max_diff = maFunctions::Highest(diff[0], diff[1]);
		float quant = max_diff / c_MaxNormalQuant;
		o_QuantLevel = quant;
		
		CodeType *ptr = o_pCodes;
		maVector3d normalized;
		envType::UInt16 sign_bit = 0;
		const float half_quant = quant * 0.5f;
		maVector3d offset(half_quant, half_quant, half_quant); // offset half quant level for better rounding
		for (int i=0; i<num_pts; ++i)
		{
			// only encode the sign of the z component in the 16th bit
			// of the Y component.
			sign_bit = (i_Deltas[i].m_Z < 0) ? c_SignBit : 0;

			normalized = offset + i_Deltas[i] - min_corner;
			if (quant > 0)
			{
				(*ptr++) = (CodeType)(normalized[0] / quant);
				(*ptr++) = (CodeType)(normalized[1] / quant) | sign_bit;
			}
			else
			{
				(*ptr++) = 0;
				(*ptr++) = 0 | sign_bit;
			}
		}

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DecodeNormalVectors(const CodeType* i_pCodes,
							float i_QuantLevel,
							const maVector3d& i_MinBounds,
							std::vector<maPoint3d> &o_Deltas)
	{
		const int num_pts = o_Deltas.size();

		const CodeType *ptr = i_pCodes;
		maVector3d normalized;
		envType::UInt16 sign_bit = 0;
		for (int i=0; i<num_pts; ++i)
		{			
			o_Deltas[i].m_X = (*ptr++) * i_QuantLevel + i_MinBounds.m_X;
			sign_bit = (*ptr) & c_SignBit; // Pick off sign of Z component
			o_Deltas[i].m_Y = (*ptr++ & c_SignMask) * i_QuantLevel + i_MinBounds.m_Y;

			// Z is set to either +1 or -1, its length can be computed
			// from the X and Y components since normals are unit length
			o_Deltas[i].m_Z = (sign_bit) ? -1.0f : 1.0f;
		}
	}

	////--------------------------------------------------------------------
	//// Given two surrounding frames and the coding for the delta vectors,
	//// reconstruct the interpolated frame.
	////--------------------------------------------------------------------
	//void DecodeInterpolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
	//					 const vtxVertexFrame &i_Frame2,
	//					 const std::vector<CodeType> &i_Codes,
	//					 const maAxisBox &i_Bounds,
	//					 vtxVertexFrame &o_Frame1)
	//{
	//	int num_pts = i_Frame0.m_Positions.size();
	//	o_Frame1.m_Positions.resize(num_pts);

	//	maPoint3d min_corner = i_Bounds.GetBoxPoint(7);
	//	maVector3d diff = i_Bounds.GetBoxPoint(0) - min_corner;

	//	float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
	//	float quant = max_diff / c_ErrorThreshold;
	//	if (quant > c_MaxQuant) 
	//		quant = c_MaxQuant;

	//	const CodeType *ptr = &i_Codes[0];
	//	maVector3d delta, normalized;
	//	for (int i=0; i<num_pts; ++i)
	//	{			
	//		if (quant > 0)
	//		{
	//			normalized[0] = (*ptr++) * diff[0] / quant;
	//			normalized[1] = (*ptr++) * diff[1] / quant;
	//			normalized[2] = (*ptr++) * diff[2] / quant;
	//			delta = normalized + min_corner;
	//		}
	//		else
	//			delta = min_corner;

	//		o_Frame1.m_Positions[i] = delta + (i_Frame2.m_Positions[i] + i_Frame0.m_Positions[i]) * 0.5f;
	//	}

	//	//DBG_LOG("Bounds: " << bounds.GetRadius());
	//}

	////--------------------------------------------------------------------
	//// Given two previous frames and the coding for the delta vectors,
	//// reconstruct the next frame.
	////--------------------------------------------------------------------
	//void DecodeExtrapolatedDeltaVectors(const vtxVertexFrame &i_Frame0,
	//					 const vtxVertexFrame &i_Frame1,
	//					 const std::vector<CodeType> &i_Codes,
	//					 const maAxisBox &i_Bounds,
	//					 vtxVertexFrame &o_Frame2)
	//{
	//	int num_pts = i_Frame0.m_Positions.size();
	//	o_Frame2.m_Positions.resize(num_pts);

	//	maPoint3d min_corner = i_Bounds.GetBoxPoint(7);
	//	maVector3d diff = i_Bounds.GetBoxPoint(0) - min_corner;

	//	float max_diff = maFunctions::Highest(diff[0], diff[1], diff[2]);
	//	float quant = max_diff / c_ErrorThreshold;
	//	if (quant > c_MaxQuant) 
	//		quant = c_MaxQuant;

	//	const CodeType *ptr = &i_Codes[0];
	//	maVector3d delta, normalized;
	//	for (int i=0; i<num_pts; ++i)
	//	{			
	//		if (quant > 0)
	//		{	
	//			normalized[0] = (*ptr++) * diff[0] / quant;
	//			normalized[1] = (*ptr++) * diff[1] / quant;
	//			normalized[2] = (*ptr++) * diff[2] / quant;
	//			delta = normalized + min_corner;
	//		}
	//		else
	//			delta = min_corner;

	//		o_Frame2.m_Positions[i] = delta + ( 2*i_Frame1.m_Positions[i] - i_Frame0.m_Positions[i]);
	//	}

	//	//DBG_LOG("Bounds: " << bounds.GetRadius());
	//}
}

