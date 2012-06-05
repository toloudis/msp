/*****************************************************************************
**	matTextureTracking.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matTextureTracking.hpp"

//============================================================================
//============================================================================
namespace matTextureTracking
{
	namespace
	{
		std::set<fsLocator> l_MissingTextureSet;
	}

	//--------------------------------------------------------------------
	//	AddMissingTexture - when the texture manager skips a texture,
	//	it should add the locator through this function.
	//--------------------------------------------------------------------
	void AddMissingTexture(const fsLocator& i_MissingTexture)
	{
		l_MissingTextureSet.insert(i_MissingTexture);
	}

	//--------------------------------------------------------------------
	// Returns access to set of missing textures
	//--------------------------------------------------------------------
	std::set<fsLocator>& GetMissingTextureSet()
	{
		return l_MissingTextureSet;
	}

	//--------------------------------------------------------------------
	// Clear - clears set of missing texture to begin new tracking set.
	//--------------------------------------------------------------------
	void Clear()
	{
		l_MissingTextureSet.clear();
	}

} // end of namespace

