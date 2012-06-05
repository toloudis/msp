/****************************************************************************\
**  hairImport.hpp
**
**      hairImport.hpp supplies functions used to import files from Maya
**	(written by our Maya plugin).
**
**		When refactored correctly, hairReader will specialize in reading
**	files into data structures for processing and mayImport will specialize 
**	in reading files into our graphics objects in order to be rendered.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef HAIR_IMPORT_HPP
#error hairImport.hpp multiply included
#endif
#define HAIR_IMPORT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <vector>


class g3dSceneNode;
class g3dFragment;
class matMaterial;
struct mdlHairInfo;

//----------------------------------------------------------------------------
//	Any of these hairImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace hairImport
{
	//read in the file, parse and convert to
	void LoadHair( const fsLocator& i_Locator,
				   mdlHairInfo &o_HairInfo,
				   mdlMatInfoTable& o_MaterialTable,
				   std::vector<matMaterial*>& o_Materials );
}