/*****************************************************************************
**	vtxVertexAnimKeysStatic.hpp
**
**		vtxVertexAnimKeysStatic - animation information for vertex baked
**	animation. This Static version holds the whole animation into memory
**	at once.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMKEYSSTATIC_HPP
#error vtxVertexAnimKeysStatic.hpp multiply included
#endif
#define VTX_VERTEXANIMKEYSSTATIC_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 
#ifndef VTX_VERTEXANIMKEYS_HPP
#include "Graphics/vtx/vtxVertexAnimKeys.hpp"
#endif 
#ifndef VTX_VERTEXFRAME_HPP
#include "Graphics/vtx/vtxVertexFrame.hpp"
#endif 


//============================================================================
//============================================================================
class vtxVertexAnimKeysStatic : public vtxVertexAnimKeys
{
	public:

		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		vtxVertexAnimKeysStatic();

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
		anKeyDataBase<vtxVertexFrame*>& Frames();
		const anKeyDataBase<vtxVertexFrame*>& GetFrames() const;

		//--------------------------------------------------------------------
		//	Mutators.  The vtxVertexAnimKeysStatic makes a copy of the anKeyData
		//	part of the animation, but does not own the vtxVertexFrame data
		//	being pointed at. This allows sharing of vertex data between
		//	animations.
		//--------------------------------------------------------------------
		//void SetFrames(anKeyDataBase<vtxVertexFrame*>& i_Anim);

		//--------------------------------------------------------------------
		// GetBracketingFrames() - get vertex anim frame before and after 
		// given time. Also returns alpha value for frame between
		// the bracketing frames to be used for blending.
		//--------------------------------------------------------------------
		void GetBracketingFrames(float i_Frame,
								 vtxVertexFrame* &o_pFrame0,
								 vtxVertexFrame* &o_pFrame1,
								 float &o_Alpha) const;

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

	private:
		anKeyDataBase<vtxVertexFrame*> m_Frames;
		anKeyDataBase<maAxisBox> m_BBoxAnim;
};


//============================================================================
// Class for holding all the frames that we loaded, no dynamic management
//============================================================================
class vtxVertexFramesStatic : public vtxVertexFrames
{
public:
	~vtxVertexFramesStatic();
	//Add this frame to the collection
	void AddVertexFrame(vtxVertexFrame* i_pFrame);
	//Get the number of frames stored
	int GetNumVertexFrames() const 
	{ 
		return m_VertexFrames.size();
	}
	//Get the i'th vertex frame stored
	vtxVertexFrame * GetVertexFrame ( int i ) const
	{
		DBG_ASSERT( i < m_VertexFrames.size(), "vertexframe index out of ramge " << i << " < " << m_VertexFrames.size() );
		if (i >= m_VertexFrames.size())
			return NULL;
		return m_VertexFrames[i ];
	}
private:
	std::vector<vtxVertexFrame*> m_VertexFrames;
};

//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysStatic::HasAnimation() const
{
	return (m_Frames.GetNumKeys() > 0);
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyDataBase<vtxVertexFrame*>&
vtxVertexAnimKeysStatic::Frames()
{
	return m_Frames;
}

inline const anKeyDataBase<vtxVertexFrame*>&
vtxVertexAnimKeysStatic::GetFrames() const
{
	return m_Frames;
}

//--------------------------------------------------------------------
// Bounding box animation
//--------------------------------------------------------------------
inline bool	vtxVertexAnimKeysStatic::HasBBoxAnimation() const
{
	// could be implemented later, but we are using dynamic keys 
	// always except in test programs anyway
	return false;
}
inline anKeyDataBase<maAxisBox>& vtxVertexAnimKeysStatic::BBoxAnim()
{
	return m_BBoxAnim;
}
inline const anKeyDataBase<maAxisBox>& vtxVertexAnimKeysStatic::BBoxAnim() const
{
	return m_BBoxAnim;
}

