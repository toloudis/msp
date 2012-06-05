/*****************************************************************************
**  emdlVertexAnimKeys.hpp
**
**      emdlVertexAnimKeys - class for maintaining shared keyframe data
**	for baked vertex animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_VERTEXANIMKEYS_HPP
#error emdlVertexAnimKeys.hpp multiply included
#endif
#define EMDL_VERTEXANIMKEYS_HPP

#ifndef ENT_ANIMKEYS_HPP
#include "Graphics/ent/entAnimKeys.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif


//============================================================================
//	emdlVertexAnimKeys
//============================================================================
class emdlVertexAnimKeys : public entAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	emdlVertexAnimKeys constructor
		//--------------------------------------------------------------------
		emdlVertexAnimKeys();

		//--------------------------------------------------------------------
		//	destructor
		//--------------------------------------------------------------------
		virtual ~emdlVertexAnimKeys();

		//--------------------------------------------------------------------
		// The vertex keys are grouped by mesh and are then
		//	keyed by frame to a list of positions and normals.
		//--------------------------------------------------------------------
		inline const std::vector<smdlVertexAnimKeys>& GetVertexKeys() const;
		inline std::vector<smdlVertexAnimKeys>& VertexKeys();

		//--------------------------------------------------------------------
		// This array of vertex frames is the resource that owns the memory
		//	of the vertex animation. The Keys above just provide a view to 
		//	this frame data.
		//--------------------------------------------------------------------
		inline smdlVertexFrames& VertexFrames();

	private:
		std::vector<smdlVertexAnimKeys> m_VertexKeys;
		smdlVertexFrames m_VertexFrames;

};

//--------------------------------------------------------------------
// Accessor functions
//--------------------------------------------------------------------
inline const std::vector<smdlVertexAnimKeys>& emdlVertexAnimKeys::GetVertexKeys() const
{
	return m_VertexKeys;
}
inline std::vector<smdlVertexAnimKeys>& emdlVertexAnimKeys::VertexKeys()
{
	return m_VertexKeys;
}
inline smdlVertexFrames& emdlVertexAnimKeys::VertexFrames()
{
	return m_VertexFrames;
}
