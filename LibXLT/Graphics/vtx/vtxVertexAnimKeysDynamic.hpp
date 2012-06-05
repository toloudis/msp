/*****************************************************************************
**	vtxVertexAnimKeysDynamic.hpp
**
**		vtxVertexAnimKeysDynamic - animation information for vertex baked
**	animation. This Dynamic version can swap individual frames in and 
**	out of memory.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMKEYSDYNAMIC_HPP
#error vtxVertexAnimKeysDynamic.hpp multiply included
#endif
#define VTX_VERTEXANIMKEYSDYNAMIC_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 
#ifndef VTX_VERTEXFRAMESDYNAMIC_HPP
#include "Graphics/vtx/vtxVertexFramesDynamic.hpp"
#endif 


//============================================================================
//============================================================================
class vtxVertexAnimKeysDynamic : public vtxVertexAnimKeys
{
	public:

		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		vtxVertexAnimKeysDynamic(const shared_ptr<vtxDynamicVertexSet>& i_VertexSet);

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
		inline anKeyDataBase<int>& Frames();
		inline const anKeyDataBase<int>& GetFrames() const;

		//--------------------------------------------------------------------
		// Bounding box animation
		//--------------------------------------------------------------------
		inline bool	HasBBoxAnimation() const;
		inline anKeyDataBase<maAxisBox>& BBoxAnim();
		inline const anKeyDataBase<maAxisBox>& BBoxAnim() const;

		//--------------------------------------------------------------------
		// Get animated bounding box at given frame
		//--------------------------------------------------------------------
		maAxisBox GetBBox(float i_Frame);

		//--------------------------------------------------------------------
		//	Mutators.  The vtxVertexAnimKeysDynamic makes a copy of the anKeyData
		//	part of the animation, but does not own the vtxVertexFrame data
		//	being pointed at. This allows sharing of vertex data between
		//	animations.
		//--------------------------------------------------------------------
		//void SetFrames(anKeyDataBase<int>& i_Anim);

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
		anKeyDataBase<int> m_Frames;
		anKeyDataBase<maAxisBox> m_BBoxAnim;
};


//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysDynamic::HasAnimation() const
{
	return (m_Frames.GetNumKeys() > 0);
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyDataBase<int>&
vtxVertexAnimKeysDynamic::Frames()
{
	return m_Frames;
}

inline const anKeyDataBase<int>&
vtxVertexAnimKeysDynamic::GetFrames() const
{
	return m_Frames;
}

//--------------------------------------------------------------------
// Bounding box animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysDynamic::HasBBoxAnimation() const
{
	return (m_BBoxAnim.GetNumKeys() > 0);
}
inline anKeyDataBase<maAxisBox>& vtxVertexAnimKeysDynamic::BBoxAnim()
{
	return m_BBoxAnim;
}
inline const anKeyDataBase<maAxisBox>& vtxVertexAnimKeysDynamic::BBoxAnim() const
{
	return m_BBoxAnim;
}
