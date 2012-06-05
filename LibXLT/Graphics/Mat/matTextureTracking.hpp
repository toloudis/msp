/****************************************************************************\
**  matTextureTracking.hpp
**
**      matTextureTracking keeps track of textures that were skipped 
**	when loading because the asset wasn't found.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURETRACKING_HPP
#error matTextureTracking.hpp multiply included
#endif
#define MAT_TEXTURETRACKING_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <set>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace matTextureTracking
{
	//--------------------------------------------------------------------
	//	AddMissingTexture - when the texture manager skips a texture,
	//	it should add the locator through this function.
	//--------------------------------------------------------------------
	void AddMissingTexture(const fsLocator& i_MissingTexture);

	//--------------------------------------------------------------------
	// Returns access to set of missing textures
	//--------------------------------------------------------------------
	std::set<fsLocator>& GetMissingTextureSet();

	//--------------------------------------------------------------------
	// Clear - clears set of missing texture to begin new tracking set.
	//--------------------------------------------------------------------
	void Clear();
}

