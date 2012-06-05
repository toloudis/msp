/*****************************************************************************
**	ExportUtil.hpp
**
**		API for Exporting a scene
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EXPORTUTILS_HPP
#error ExportUtil.hpp multiply included
#endif
#define EXPORTUTILS_HPP

//Forward Declarations
class fsLocator;

namespace ExportUtil
{
	//------------------------------------------------------------------------
	//  AddToMenu() - add export command to menu
	//------------------------------------------------------------------------
	void  AddToMenu();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BeginExport();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void GetExportFile();

	//------------------------------------------------------------------------
	// Gathers the scene resources from the resource tracker and 
	// sets up the directory
	//------------------------------------------------------------------------
	void GatherSceneResources();

	//------------------------------------------------------------------------
	// Package each resource file and add it to the archive
	//------------------------------------------------------------------------
	bool PackageScene(const fsLocator& i_SourceFile);

} // end namespace ExportUtil