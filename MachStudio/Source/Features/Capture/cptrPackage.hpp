/*****************************************************************************
**  cptrPackage.hpp
**
**      Initializes feature Capture
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_PACKAGE_HPP
#error cptrPackage.hpp multiply included
#endif
#define CPTR_PACKAGE_HPP


//============================================================================
//============================================================================
class g2dSystem;
class fsLocator;


//============================================================================
//============================================================================
namespace cptrPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem);

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (batch)
	//
	//	batch file format: one scene name per line
	//--------------------------------------------------------------------
	void LaunchBatchRender(const fsLocator& i_BatchFile, bool i_bClose = true);

	//--------------------------------------------------------------------
	//	Launch the capture for the current scene (bake texture)
	//--------------------------------------------------------------------
	//void LaunchBakeRender(bool i_bClose = true);

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//
	//	i_bClearModeStack - set to false is the call stack (usually 
	//	containing "manipulation mode" should not be cleared.)
	//--------------------------------------------------------------------
	void LaunchRender(const fsLocator& i_SceneFile, bool i_bClearModeStack = true);

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//--------------------------------------------------------------------
	void LaunchRender();

	//--------------------------------------------------------------------
	//	return true if the current mode is one of the render modes
	//--------------------------------------------------------------------
	bool IsRenderModeActive();
};
