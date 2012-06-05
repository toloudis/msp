/*****************************************************************************
**  cptrPackage.hpp
**
**      Initializes feature Capture
**
**	Extra Large Technology
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
	void LaunchBatchRender(fsLocator& i_BatchFile);

	//--------------------------------------------------------------------
	//	Launch the render with the currently loaded scene
	//--------------------------------------------------------------------
	void LaunchRender(int width, int height);
	void LaunchFrameRender(int width, int height);
	void LaunchProgressiveFrameRender(int startWidth, int startHeight, int width, int height);

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//--------------------------------------------------------------------
	void LaunchRender(fsLocator& i_SceneFile, int width, int height);
	void LaunchQuickRender(fsLocator& i_SceneFile, int width, int height);
};
