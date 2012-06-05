/*****************************************************************************
**	vtxVertexAnimKeysCompressed.hpp
**
**		vtxVertexAnimKeysCompressed - animation information for vertex baked
**	animation. This Compressed version can swap individual frames in and 
**	out of memory.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMKEYSCOMPRESSED_HPP
#error vtxVertexAnimKeysCompressed.hpp multiply included
#endif
#define VTX_VERTEXANIMKEYSCOMPRESSED_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 
#ifndef VTX_VERTEXFRAMESCOMPRESSED_HPP
#include "Graphics/vtx/vtxVertexFramesCompressed.hpp"
#endif 


//============================================================================
//============================================================================
class vtxVertexAnimKeysCompressed : public vtxVertexAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		struct sFrameData
		{
			int m_ReferenceFrame;
			int m_DeltaBlock;
			int m_DeltaOffset;
			maAxisBox m_BBox;
			bool m_bStatic;

			sFrameData() : m_ReferenceFrame(0), m_DeltaBlock(-1), m_DeltaOffset(0), m_bStatic(false) {}
		};

		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		vtxVertexAnimKeysCompressed(const shared_ptr<vtxDynamicVertexSet> i_VertexSet,
									const shared_ptr<vtxCompressedDeltasSet> i_DeltasSet);
	
		//--------------------------------------------------------------------
		//	HasAnimation - returns true if some channel has animation
		//--------------------------------------------------------------------
		inline bool	HasAnimation() const;

		//--------------------------------------------------------------------
		//	GetAnimLength returns the time value for the last key
		//--------------------------------------------------------------------
		float GetAnimLength() const;

		//--------------------------------------------------------------------
		// Get number of vertices in a frame of animation data.
		// Used to make sure the vertex data is compatible with the
		// geometry.
		//--------------------------------------------------------------------
		void GetNumVertices(int &o_NumVertices, int &o_NumNormals);

		//--------------------------------------------------------------------
		//	Accessors - const and non-const.
		//--------------------------------------------------------------------
		inline anKeyDataBase<sFrameData>& Frames();
		inline const anKeyDataBase<sFrameData>& GetFrames() const;

		//--------------------------------------------------------------------
		// Bounding box animation
		//--------------------------------------------------------------------
		inline bool	HasBBoxAnimation() const;

		//--------------------------------------------------------------------
		// Get animated bounding box at given frame
		//--------------------------------------------------------------------
		maAxisBox GetBBox(float i_Frame);

		//--------------------------------------------------------------------
		// GetBracketingFrames() - get vertex anim frame before and after 
		// given time. Also returns alpha value for frame between
		// the bracketing frames to be used for blending.
		//--------------------------------------------------------------------
		void GetBracketingFrames(float i_Frame,
								 vtxVertexFrame* &o_pFrame0,
								 vtxVertexFrame* &o_pFrame1,
								 float &o_Alpha) const;
	private:
		shared_ptr<vtxDynamicVertexSet> m_VertexSet;
		shared_ptr<vtxCompressedDeltasSet> m_DeltasSet;
		anKeyDataBase<sFrameData> m_Frames;
};


//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysCompressed::HasAnimation() const
{
	return (m_Frames.GetNumKeys() > 0);
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyDataBase<vtxVertexAnimKeysCompressed::sFrameData>&
vtxVertexAnimKeysCompressed::Frames()
{
	return m_Frames;
}

inline const anKeyDataBase<vtxVertexAnimKeysCompressed::sFrameData>&
vtxVertexAnimKeysCompressed::GetFrames() const
{
	return m_Frames;
}

//--------------------------------------------------------------------
// Bounding box animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysCompressed::HasBBoxAnimation() const
{
	// There is no variation of this animation that does not
	// have bounding box info, so return true if we have
	// animation at all.
	return (m_Frames.GetNumKeys() > 0);
}

