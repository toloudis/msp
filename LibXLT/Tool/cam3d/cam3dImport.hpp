/****************************************************************************\
**	cam3dImport.hpp
**
**		cam3dImport supplies functions used to import camera scripts
**	from Maya (written by our Maya plugin).
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CAM3D_IMPORT_HPP
#error cam3dImport.hpp multiply included
#endif
#define CAM3D_IMPORT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif
#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class fsLocator;
class cam3dAnimKeys;


//============================================================================
//============================================================================
namespace cam3dImport
{
	//------------------------------------------------------------------------
	//	LoadAnimation loads an camera script animation from a file and
	//		returns new script info. Ownership passes to caller.
	//		If there is an error, NULL is returned.
	//------------------------------------------------------------------------
	cam3dAnimKeys* LoadAnimation( const fsLocator& i_Locator,
								  float &o_FramesPerSecond,
								  float &o_BeginFrame );
}
