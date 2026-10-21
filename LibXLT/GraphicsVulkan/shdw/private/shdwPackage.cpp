/*****************************************************************************
**  shdwPackage.cpp
**
**      shdwPackage contains the initialization and cleanup functions
**	for the shdw package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_PACKAGE_HPP
#include "GraphicsVulkan/shdw/shdwPackage.hpp"
#endif
#ifndef GVK_SHDW_SCENERENDERERCREATE_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererCreate.hpp"
#endif
#ifndef GVK_SHDW_PASSTONEMAP_HPP
#include "GraphicsVulkan/shdw/shdwPassToneMap.hpp"
#endif
#ifndef GVK_SHDW_PASSAMBIENT_HPP
#include "GraphicsVulkan/shdw/shdwPassAmbient.hpp"
#endif
#ifndef GVK_SHDW_PASSENVIRONMENT_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvironment.hpp"
#endif
#ifndef GVK_SHDW_PASSLIT_HPP
#include "GraphicsVulkan/shdw/shdwPassLit.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_SHDW_PASSALPHAFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassAlphaFill.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef GVK_SHDW_SHADOWLAYERRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowLayerRendererVk.hpp"
#endif
#ifndef GVK_SHDW_HDRRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwHDRRendererVk.hpp"
#endif
#ifndef GVK_SHDW_PASSHAIR_HPP
#include "GraphicsVulkan/shdw/shdwPassHair.hpp"
#endif
#ifndef GVK_SHDW_PASSENVBACKGROUND_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvBackground.hpp"
#endif
#ifndef GVK_SHDW_PASSSSAO_HPP
#include "GraphicsVulkan/shdw/shdwPassSSAO.hpp"
#endif
#ifndef GVK_SHDW_PASSGLOW_HPP
#include "GraphicsVulkan/shdw/shdwPassGlow.hpp"
#endif
#ifndef GVK_SHDW_PASSOUTLINE_HPP
#include "GraphicsVulkan/shdw/shdwPassOutline.hpp"
#endif
#ifndef GVK_SHDW_PASSPOSTSHADER_HPP
#include "GraphicsVulkan/shdw/shdwPassPostShader.hpp"
#endif
#ifndef GVK_SHDW_PASSFOG_HPP
#include "GraphicsVulkan/shdw/shdwPassFog.hpp"
#endif
#ifndef GVK_SHDW_PASSDOF_HPP
#include "GraphicsVulkan/shdw/shdwPassDOF.hpp"
#endif
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif
#ifndef GVK_SHDW_PASSSSGI_HPP
#include "GraphicsVulkan/shdw/shdwPassSSGI.hpp"
#endif
#ifndef GVK_SHDW_PASSLPVGI_HPP
#include "GraphicsVulkan/shdw/shdwPassLPVGI.hpp"
#endif
#ifndef GVK_SHDW_PASSAOVOLUMES_HPP
#include "GraphicsVulkan/shdw/shdwPassAOVolumes.hpp"
#endif

#ifndef GVK_SHDW_NORMALRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwNormalRendererVk.hpp"
#endif
#ifndef GVK_SHDW_MATERIALSRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwMaterialsRendererVk.hpp"
#endif
#ifndef GVK_SHDW_PASSMATERIALS_HPP
#include "GraphicsVulkan/shdw/shdwPassMaterials.hpp"
#endif
#ifndef GVK_SHDW_DEPTHRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwDepthRendererVk.hpp"
#endif
#ifndef GVK_SHDW_REFLECTIONSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwReflectionsOnlyRendererVk.hpp"
#endif
#ifndef GVK_SHDW_GLOWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGlowRendererVk.hpp"
#endif

#include <cstddef>
namespace gvk
{

namespace
{

int l_RefCount = 0;
shdwSceneRendererCreate* l_pRendererCreator = 0;

}

//------------------------------------------------------------------------
//	Init must be called before you use the shdw package.  A good place to
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
//	CleanUp should be called after you are done with the shdw package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void shdwPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order (DX11 leaves the
		// deleted creator installed; it is removed here)
		//
		g3dSceneRendererCreate::SetImplementation(NULL);
		delete l_pRendererCreator;
		l_pRendererCreator = NULL;

		// clean up packages we depend on
	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//	(the InitStates of each pass and renderer joins here, in DX11's order,
//	as it is ported)
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
	shdwNormalRendererVk::InitStates();
	shdwPassZFill::InitStates();
	shdwMaterialsRendererVk::InitStates();
	shdwPassMaterials::InitStates();
	shdwDepthRendererVk::InitStates();
	shdwShadowLayerRendererVk::InitStates();
	shdwPassOutline::InitStates();
	shdwPassGlow::InitStates();
	shdwPassFog::InitStates();
	shdwPassDOF::InitStates();
	shdwHDRRendererVk::InitStates();
	shdwReflectionsOnlyRendererVk::InitStates();
	shdwPassHair::InitStates();
	shdwPassAOVolumes::InitStates();
	shdwPassLPVGI::InitStates();
	shdwGlowRendererVk::InitStates();
	shdwPassPostShader::InitStates();
	shdwPassEnvBackground::InitStates();
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void shdwPackage::CleanUpGraphics()
{
	shdwPassEnvBackground::CleanupStates();
	shdwPassPostShader::CleanupStates();
	shdwGlowRendererVk::CleanupStates();
	shdwPassLPVGI::CleanupStates();
	shdwPassAOVolumes::CleanupStates();
	shdwPassHair::CleanupStates();
	shdwReflectionsOnlyRendererVk::CleanupStates();
	shdwHDRRendererVk::CleanupStates();
	shdwPassDOF::CleanupStates();
	shdwPassFog::CleanupStates();
	shdwPassGlow::CleanupStates();
	shdwPassOutline::CleanupStates();
	shdwShadowLayerRendererVk::CleanupStates();
	shdwDepthRendererVk::CleanupStates();
	shdwPassMaterials::CleanupStates();
	shdwMaterialsRendererVk::CleanupStates();
	shdwPassZFill::CleanupStates();
	shdwNormalRendererVk::CleanupStates();
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

	shdwPassSSGI::CleanUp();
	shdwPassSSAO::CleanUp();
}

}
