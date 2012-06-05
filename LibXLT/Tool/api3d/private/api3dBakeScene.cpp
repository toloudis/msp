/*****************************************************************************
**	api3dBakeScene.cpp
**
**	Creates and owns lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dBakeScene.hpp"

#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/g2d/g2dPFD.hpp"
#include "Graphics/g3d/g3dBake.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneTraverse.hpp"
#include "Tool/gui/guiProgressDialog.hpp"


//============================================================================
//============================================================================
namespace api3dBakeScene
{

//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
void Initialize()
{
}

//--------------------------------------------------------------------
// DeInitialize
//--------------------------------------------------------------------
void DeInitialize()
{
}

//--------------------------------------------------------------------
//  Bake!
//--------------------------------------------------------------------
void BakeScene(BakeFormat i_Format, g3dScene* i_pScene, const fsLocator& i_OutputPath,
			   float i_fSimTime, const camCamera& i_Camera,
			   std::map<const g3dFragment*, std::string> &io_TextureNameMap, bool i_bIsSaveAndReplace, 
			   const std::string& i_OutputFormat, int i_Res,
			   int i_TextureReduce)
{
	g2dPFD pfd(g2dPFD::e_Color, 32);
	switch (i_Format)
	{
	case e_RGBA8:
		pfd = g2dPFD(g2dPFD::e_Color, 32);
		// if e_Color, need to supply alpha info, otherwise you get X8R8G8B8
		pfd.Set(16,8,8,8,0,8,24,8,32);
		break;
	case e_RGBA16f:
		pfd = g2dPFD(g2dPFD::e_RGBA16f, 16*4);
		break;
	}

	
	int n = g3dBake::Init(pfd, i_pScene, i_OutputPath, i_fSimTime, i_Camera, i_bIsSaveAndReplace, i_OutputFormat, i_Res, i_TextureReduce);

	int i = 0;
	guiProgressDialog::Show("Baked Texture Export", "Baking lighting to textures...");
	while (g3dBake::BakeNextNode(io_TextureNameMap))
	{
		i++;
		if (!guiProgressDialog::SetPercentage((float)i/n))
			break;
	}

	g3dBake::CleanUp();

	//g3dBake::ReplaceMaterial();
	guiProgressDialog::Hide();
}

}	// end of namespace
