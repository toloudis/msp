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

//#include <windows.h>
//#include <string>
//#include <stdio.h>
//#include <shader.h>
//#include <geoshader.h>
//#include <mirelay.h>
//
//#pragma comment(lib,"libray.lib")
//
//static void memerror(void) { mi_fatal("out of memory"); }
//static void imgerror(miImg_file * const ifp) { mi_fatal("file error, exiting."); }

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
void rmanLive::StartRender(fsLocator i_MiFile, int i_RenderPass)
{
	//// make sure mi file exists before doing anything
	//if ( !fsFileUtil::FileExists( i_MiFile ) ) return;

	//camCamera& cam = cam3dMgr::GetCamera();

	//camPassBuffersData passBuffersData;
	//cam.GetPassBuffersParams(passBuffersData);

	//if ( i_RenderPass == rmanLiveRenderPasses::e_AmbientOcclusion )
	//{
	//	passBuffersData.m_AOBuffer = l_BufferAO;
	//}
	//else if ( i_RenderPass == rmanLiveRenderPasses::e_FinalGather )
	//{
	//	passBuffersData.m_GIBuffer = l_BufferFG;
	//}
	//else if ( i_RenderPass == rmanLiveRenderPasses::e_Reflections )
	//{
	//	passBuffersData.m_ReflBuffer = l_BufferRefl;
	//}
	//else if ( i_RenderPass == rmanLiveRenderPasses::e_Beauty )
	//{
	//	passBuffersData.m_BeautyBuffer = l_BufferBeauty;
	//}

	//cam.SetPassBuffersParams( passBuffersData );

	
//	std::string miFileLoc;
//	fsFileUtil::LocatorToANSIFilename( i_MiFile, miFileLoc);
//
//    int             nthreads;
//    miBoolean       resume = miFALSE;
//    if ( !mi_raylib_attach_process() ) return;
//
//    mi_mem_error_handler(memerror);
//    mi_img_err_handler(imgerror);
//    mi_mem_init();
//    mi_ntlib_init();
//    nthreads = mi_raylib_license_get(mi_msg_no_of_cpus());
//    mi_raylib_init(miFALSE, nthreads, miFALSE);
//
//#ifdef WIN_NT
//    mi_link_set_module_handle(GetModuleHandle("libray.dll"));
//#endif
//
//    mi_mi_parse_rayrc(0, miFALSE);
//
//	mi_mi_parse(miFileLoc.c_str(), resume, 0, 0, 0, getc, miFALSE, 0);
//    miTag root, caminst, cam, opt;
//    miInh_func inh;
//    mi_api_render_params(&root, &caminst, &cam, &opt, &inh);
//    resume = mi_rc_run(miRENDER_DEFAULT, 0, 0, root, caminst, cam, opt, inh);
//    mi_api_render_release();
//
//    mi_raylib_license_release();
//    mi_raylib_exit();
//    mi_raylib_detach_process();

//	gpxRenderControl::SetNeedsNewRender();
}

//--------------------------------------------------------------------
// StopRender()
//--------------------------------------------------------------------
void rmanLive::StopRender()
{

}