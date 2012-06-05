/*****************************************************************************
**	prtVertexAnimKeys.hpp
**
**	prtVertexAnimKeys - animation information for baked particle
**	animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_VERTEXANIMKEYS_HPP
#error prtVertexAnimKeys.hpp multiply included
#endif
#define PRT_VERTEXANIMKEYS_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
// Baked info about a particle at a given frame
// IMPORTANT: these are read as arrays of memory, 
// so the data layout in the struct is critical
//============================================================================
struct prtParticleAttr
{
	prtParticleAttr()
		: m_Position(0,0,0), m_Age(0), m_Lifespan(0), m_ID(0) {}

	envType::Int32 m_ID;
	maPoint3d m_Position;
	float m_Age;
	float m_Lifespan;
};


//============================================================================
//============================================================================
struct prtVertexFrame
{
	std::vector<prtParticleAttr> m_Particles;
};


//============================================================================
//============================================================================
class prtVertexAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		prtVertexAnimKeys();

		//--------------------------------------------------------------------
		//	HasAnimation - returns true if some channel has animation
		//--------------------------------------------------------------------
		inline bool	HasAnimation() const;

		//--------------------------------------------------------------------
		//	Accessors - const and non-const.
		//--------------------------------------------------------------------
		anKeyDataBase<prtVertexFrame*>& Frames();
		const anKeyDataBase<prtVertexFrame*>& GetFrames() const;

		//--------------------------------------------------------------------
		//	Mutators.  The prtVertexAnimKeys makes a copy of the anKeyData
		//	part of the animation, but does not own the prtVertexFrame data
		//	being pointed at. This allows sharing of vertex data between
		//	animations.
		//--------------------------------------------------------------------
		void SetFrames(anKeyDataBase<prtVertexFrame*>& i_Anim);

	private:
		anKeyDataBase<prtVertexFrame*> m_Frames;
};

//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	prtVertexAnimKeys::HasAnimation() const
{
	return (m_Frames.GetNumKeys() > 0);
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyDataBase<prtVertexFrame*>&
prtVertexAnimKeys::Frames()
{
	return m_Frames;
}

inline const anKeyDataBase<prtVertexFrame*>&
prtVertexAnimKeys::GetFrames() const
{
	return m_Frames;
}

