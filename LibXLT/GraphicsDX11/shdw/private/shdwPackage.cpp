/*****************************************************************************
**  shdwPackage.cpp
**
**      shdwPackage contains the initialization and cleanup functions
**	for the bump package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/shdw/shdwPackage.hpp"

#include "GraphicsDX11/shdw/shdwBakingRenderer.hpp"
//#include "GraphicsDX11/shdw/shdwDepthMapAO.hpp"
#include "GraphicsDX11/shdw/shdwSceneRendererCreate.hpp"
#include "GraphicsDX11/shdw/shdwGlowRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwHDRRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassAlphaFill.hpp"
#include "GraphicsDX11/shdw/shdwPassAmbient.hpp"
#include "GraphicsDX11/shdw/shdwPassAOVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/shdw/shdwPassEnvironment.hpp"
#include "GraphicsDX11/shdw/shdwPassGIVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassLit.hpp"
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"
#include "GraphicsDX11/shdw/shdwPassSSAO.hpp"
#include "GraphicsDX11/shdw/shdwPassSSGI.hpp"
#include "GraphicsDX11/shdw/shdwPassToneMap.hpp"
#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"
#include "GraphicsDX11/shdw/shdwNormalRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassVelocity.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/shdw/shdwMaterialsRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassMaterials.hpp"
#include "GraphicsDX11/shdw/shdwShadowsOnlyRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwDepthRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwAOPreviewRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwAOVolumesRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwShadowLayerRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassOutline.hpp"
#include "GraphicsDX11/shdw/shdwPassGlow.hpp"
#include "GraphicsDX11/shdw/shdwPassFog.hpp"
#include "GraphicsDX11/shdw/shdwPassDOF.hpp"
//#include "GraphicsDX11/shdw/shdwPassRSMGI.hpp"
#include "GraphicsDX11/shdw/shdwGIPreviewRendererDX11.hpp"
//#include "GraphicsDX11/shdw/shdwGIVolumesRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwVelocityMapRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwUVRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwReflectionsOnlyRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassHair.hpp"
#include "GraphicsDX11/shdw/shdwPassOpacity.hpp"
#include "GraphicsDX11/shdw/shdwOpacityMapRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassLPVGI.hpp"
#include "GraphicsDX11/shdw/shdwPassPostShader.hpp"
#include "GraphicsDX11/shdw/shdwPassEnvBackground.hpp"

namespace
{

int l_RefCount = 0;
shdwSceneRendererCreate* l_pRendererCreator = 0;

//shdwDepthMapAO* l_pDepthMapAO = 0;
shdwBakingRenderer* l_pBakingRenderer = 0;
}

//------------------------------------------------------------------------
//	Init must be called before you use the bump package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void shdwPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pRendererCreator = new shdwSceneRendererCreate;
		g3dSceneRendererCreate::SetImplementation(l_pRendererCreator);

	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the bump package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void shdwPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//

		delete l_pRendererCreator;

		// clean up packages we depend on
	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void shdwPackage::InitGraphics()
{
	shdwPassToneMap::InitStates();
	shdwPassAmbient::InitStates();
	shdwPassEnvironment::InitStates();
	shdwPassLit::InitStates();
	shdwPassNormals::InitStates();
	shdwPassDepth::InitStates();
	shdwPassAlphaFill::InitStates();
	shdwPassSSAO::InitStates();
	shdwPassSSGI::InitStates();
	shdwPassTransparent::InitStates();
	shdwNormalRendererDX11::InitStates();
	shdwPassZFill::InitStates();
	shdwMaterialsRendererDX11::InitStates();
	shdwPassMaterials::InitStates();
	shdwDepthRendererDX11::InitStates();
	shdwAOPreviewRendererDX11::InitStates();
	shdwShadowsOnlyRendererDX11::InitStates();
	shdwPassVelocity::InitStates();
	shdwShadowLayerRendererDX11::InitStates();
	shdwPassOutline::InitStates();
	shdwPassGlow::InitStates();
	shdwPassFog::InitStates();
	shdwPassDOF::InitStates();
	shdwHDRRendererDX11::InitStates();
	shdwGIPreviewRendererDX11::InitStates();
	shdwVelocityMapRendererDX11::InitStates();
	shdwUVRendererDX11::InitStates();
	shdwReflectionsOnlyRendererDX11::InitStates();
	shdwBakingRenderer::InitStates();
	shdwPassHair::InitStates();
	shdwPassOpacity::InitStates();
	shdwOpacityMapRendererDX11::InitStates();
	shdwPassAOVolumes::InitStates();
	shdwAOVolumesRendererDX11::InitStates();
	//shdwPassRSMGI::InitStates();
	shdwPassGIVolumes::InitStates();
//	shdwGIVolumesRendererDX11::InitStates();
	shdwPassLPVGI::InitStates();
	shdwGlowRendererDX11::InitStates();
	shdwPassPostShader::InitStates();
	shdwPassEnvBackground::InitStates();

//	l_pDepthMapAO = new shdwDepthMapAO;
//	g3dAmbientOcclusion::SetImplementation(l_pDepthMapAO);

	l_pBakingRenderer = new shdwBakingRenderer;
	g3dBake::SetImplementation(l_pBakingRenderer);
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void shdwPackage::CleanUpGraphics()
{
	shdwPassEnvBackground::CleanupStates();
	shdwPassPostShader::CleanupStates();
	shdwGlowRendererDX11::CleanupStates();
	shdwPassLPVGI::CleanupStates();
//	shdwPassRSMGI::CleanupStates();
//	shdwGIVolumesRendererDX11::CleanupStates();
	shdwPassGIVolumes::CleanupStates();
	shdwAOVolumesRendererDX11::CleanupStates();
	shdwPassAOVolumes::CleanupStates();
	shdwOpacityMapRendererDX11::CleanupStates();
	shdwPassOpacity::CleanupStates();
	shdwPassHair::CleanupStates();
	shdwBakingRenderer::CleanupStates();
	shdwReflectionsOnlyRendererDX11::CleanupStates();
	shdwUVRendererDX11::CleanupStates();
	shdwVelocityMapRendererDX11::CleanupStates();
	shdwGIPreviewRendererDX11::CleanupStates();
	shdwHDRRendererDX11::CleanupStates();
	shdwPassDOF::CleanupStates();
	shdwPassFog::CleanupStates();
	shdwPassGlow::CleanupStates();
	shdwPassOutline::CleanupStates();
	shdwShadowLayerRendererDX11::CleanupStates();
	shdwPassVelocity::CleanupStates();
	shdwShadowsOnlyRendererDX11::CleanupStates();
	shdwAOPreviewRendererDX11::CleanupStates();
	shdwDepthRendererDX11::CleanupStates();
	shdwPassMaterials::CleanupStates();
	shdwMaterialsRendererDX11::CleanupStates();
	shdwPassZFill::CleanupStates();
	shdwNormalRendererDX11::CleanupStates();
	shdwPassTransparent::CleanupStates();
	shdwPassSSGI::CleanupStates();
	shdwPassSSAO::CleanupStates();
	shdwPassAlphaFill::CleanupStates();
	shdwPassDepth::CleanupStates();
	shdwPassNormals::CleanupStates();
	shdwPassLit::CleanupStates();
	shdwPassEnvironment::CleanupStates();
	shdwPassAmbient::CleanupStates();
	shdwPassToneMap::CleanupStates();

	// release special textures used by these renderers
	shdwPassSSGI::CleanUp();
	shdwPassSSAO::CleanUp();

	SAFE_DELETE( l_pBakingRenderer );
	g3dBake::SetImplementation(NULL);

//	delete l_pDepthMapAO;
//	l_pDepthMapAO = NULL;
//	g3dAmbientOcclusion::SetImplementation(NULL);
}