/****************************************************************************\
**	prtImport.hpp
**
**		prtImport.hpp handles importing baked particle animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_IMPORT_HPP
#error prtImport.hpp multiply included
#endif
#define PRT_IMPORT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRT_VERTEXANIMKEYS_HPP
#include "Graphics/prt/prtVertexAnimKeys.hpp"
#endif

#include <vector>


//============================================================================
//	Any of these prtImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace prtImport
{
	//------------------------------------------------------------------------
	//	LoadVertexAnimation loads the baked vertex and normal 
	//	animation from the given file.
	//------------------------------------------------------------------------
	void LoadVertexAnimation( const fsLocator& i_Locator,
							std::vector<prtVertexAnimKeys>& o_VertexKeys,
							std::vector<prtVertexFrame*>& o_VertexFrames,
							float &o_FramesPerSecond,
							float &o_BeginFrame);
}
