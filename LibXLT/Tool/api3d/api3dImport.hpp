/*****************************************************************************
**	api3dImport.hpp
**
**	Imports objects from files
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_IMPORT_HPP
#error api3dImport.hpp multiply included
#endif
#define API3D_IMPORT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dParticleGenerator;
class entModelTemplate;


//============================================================================
//============================================================================
namespace api3dImport
{
	//--------------------------------------------------------------------
	//	load a model template for the passed in file.
	//--------------------------------------------------------------------
	entModelTemplate* LoadModelTemplate( const fsLocator& i_LODFileLocator, const std::string& i_ModelFile );

	//--------------------------------------------------------------------
	//  Loads data from file and creates object of appropriate type
	//--------------------------------------------------------------------
	api3dObject * LoadObject(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	//  Loads data from file and creates a particle generator based on
	//	a TPR file
	//--------------------------------------------------------------------
	api3dParticleGenerator* CreateParticleGenerator(const fsLocator &i_Locator);

}	// end of namespace
