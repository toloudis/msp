/*****************************************************************************
**	mslMetaSLMgr.hpp
**
**	 mslMetaSLMgr maintains the entry point to the MetaSL library.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_METASL_HPP
#error mslMetaSLMgr.hpp multiply included
#endif
#define MSL_METASL_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

class matMaterial;

//============================================================================
//============================================================================
namespace mslMetaSLMgr 
{
	//--------------------------------------------------------------------
	// Initialize
	//--------------------------------------------------------------------
	bool  Initialize(const fsLocator& i_DataDirectory);

	//--------------------------------------------------------------------
	// DeInitialize
	//--------------------------------------------------------------------
	void  DeInitialize();

	//--------------------------------------------------------------------
	// Clear out any existing node graph
	//--------------------------------------------------------------------
	void ClearGraph();

	//--------------------------------------------------------------------
	// Create and load a graph into the application
	//--------------------------------------------------------------------
	bool LoadGraph(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	// Save a graph under a given file name in the application
	//--------------------------------------------------------------------
	void SaveGraph(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	// Layout graph nodes automatically.
	//--------------------------------------------------------------------
	void LayoutGraph();

	//--------------------------------------------------------------------
	// Pan and zoom such that whole graph is visible.
	//--------------------------------------------------------------------
	void ZoomExtents();

	//--------------------------------------------------------------------
	// Move cursor to given source line number in shader output window
	//--------------------------------------------------------------------
	void GotoSourceLine(int i_LineNum);

	//--------------------------------------------------------------------
	// Delete selected nodes
	//--------------------------------------------------------------------
	void DeleteSelectedNodes();

	//--------------------------------------------------------------------
	// Disconnect inputs or outputs from selected nodes
	//--------------------------------------------------------------------
	void DisconnectInputs();
	void DisconnectOutputs();
	
	//--------------------------------------------------------------------
	// Return directory containing MetaSL textures
	//--------------------------------------------------------------------
	const fsLocator& GetTextureDirectory();

	//--------------------------------------------------------------------
	// Compile node graph into HLSL shader 
	// and put into the given material.
	// If i_EffectFilename is given, write binary FX to this filename.
	//--------------------------------------------------------------------
	bool Compile(matMaterial* i_pShaderMaterial,
				 const fsLocator& i_EffectFilename);

}
