/****************************************************************************\
**	rmanLive.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/rman/live/rmanLive.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"

//--------------------------------------------------------------------
// Anonymous namespace, owns the bufers
//--------------------------------------------------------------------
namespace
{
	matTexture* l_BufferAO = NULL;
	matTexture* l_BufferFG = NULL;
	matTexture* l_BufferRefl = NULL;
	matTexture* l_BufferBeauty = NULL;
}

//--------------------------------------------------------------------
// CleanUp()
//--------------------------------------------------------------------
void rmanLive::CleanUp()
{
	matTextureMgr::ReleaseTexture( l_BufferAO );
	matTextureMgr::ReleaseTexture( l_BufferFG );
	matTextureMgr::ReleaseTexture( l_BufferRefl );
	matTextureMgr::ReleaseTexture( l_BufferBeauty );
}

//--------------------------------------------------------------------
// StartRender()
//--------------------------------------------------------------------
void rmanLive::StartRender(fsLocator i_SceneFile, int i_RenderPass)
{
}

//--------------------------------------------------------------------
// StopRender()
//--------------------------------------------------------------------
void rmanLive::StopRender()
{

}