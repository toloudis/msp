/*****************************************************************************
**	api3dBakeScene.hpp
**
**	Creates and owns lights
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_BAKESCENE_HPP
#error api3dBakeScene.hpp multiply included
#endif
#define API3D_BAKESCENE_HPP

#include <map>
#include <string>


//============================================================================
//============================================================================
class camCamera;
class fsLocator;
class g3dScene;
class g3dFragment;


//============================================================================
//============================================================================
namespace api3dBakeScene
{
	enum BakeFormat {e_RGBA16f, e_RGBA8};

	//--------------------------------------------------------------------
	// Initialize
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	// DeInitialize
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	//  Bake!
	//	possible usage: 		
	//		std::map<const g3dFragment*, std::string> texturename_map;
	//		api3dBakeScene::BakeScene(api3dBakeScene::e_RGBA16f, api3dScene::GetScene(), fsLocator(),
	//					appSimTime::GetTime(), cam3dMgr::GetCamera(), texturename_map );
	//
	//	The texture name map can be used to assign texture names to use when
	//	baking, or to just gather the names that were generated.
	//
	//	Texture reduce says how many powers of two to reduce the main texture
	//  in order to produce the size of the baked texture. Each level
	//	reduces an edge by two and memory use by 4.
	//--------------------------------------------------------------------
	void  BakeScene(BakeFormat i_Format, g3dScene* i_pScene, const fsLocator& i_OutputPath, 
		float i_fSimTime, const camCamera& i_Camera, 
		std::map<const g3dFragment*, std::string> &io_TextureNameMap, bool i_bIsSaveAndReplace,
		const std::string& i_OutputFormat, int i_Res,
		int i_TextureReduce = 0);

}	// end of namespace
