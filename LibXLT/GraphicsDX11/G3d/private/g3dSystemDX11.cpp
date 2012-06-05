/****************************************************************************\
**	g3dSystemDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g3d/g3dSystemDX11.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dFontUtilDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dScreenCaptureUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "GraphicsDX11/eff/effShaderUtilWin.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#include "GraphicsDX11/g3d/g3dStateMgr.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matTextureMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/G3d/g3dDepthMapRendererDX11.hpp"
#include "GraphicsDX11/G3d/g3dPickBufferRendererDX11.hpp"
#include "GraphicsDX11/G3d/g3dRenderCameraSpace.hpp"
#include "GraphicsDX11/G3d/g3dRenderScreenSpace.hpp"
#include "GraphicsDX11/G3d/g3dRSMRendererDX11.hpp"
#include "GraphicsDX11/G3d/g3dSceneRendererDX11.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/G3d/g3dVSMRendererDX11.hpp"

//#include "profile.h"


//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{
	// Mip map
	float l_fMipMapLODBias = -1;

	g3dBlendStateMgr::BlendState* st_pInitial = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_pTest_Write_LessE_NS = NULL;

	//------------------------------------------------------------------------
	//	init_d3d_states
	//------------------------------------------------------------------------
	void init_d3d_states()
	{
		//initialize State managers first
		g3dBlendStateMgr::Init();
		g3dDepthStencilStateMgr::Init();
		g3dRasterizerStateMgr::Init();

		g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

		st_pInitial = new g3dBlendStateMgr::BlendState( false, FALSE,
			D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
			D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL );
		ds_pTest_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
			TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );

		//init g3d state systems
		g3dStateMgrDX11::Init();
		g3dDX11Util::InitStates();
		g3dDepthMapRendererDX11::InitStates();
		g3dPickBufferRendererDX11::InitStates();
		g3dRenderCameraSpaceObjects::InitStates();
		g3dRenderScreenSpaceObjects::InitStates();
		g3dSceneRendererDX11::InitStates();
		g3dTransparencySortDX11::InitStates();
		matTextureMgrDX11::InitStates();
		g3dRenderPass::InitStates();
		g3dVSMRendererDX11::InitStates();
		g3dRSMRendererDX11::InitStates();

		//	we want to set D3D to cull CW faces
		//	(because of our coordinate system change to right-handed), even though
		//	our users think that we are culling CCW faces
		//
		g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_FRONT, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		//	Set Z comparison
		//
		g3dDepthStencilStateMgr::SetDepthStencilState( ds_pTest_Write_LessE_NS );

		//	set blend states
		g3dBlendStateMgr::SetBlendState( st_pInitial );
	}

	void cleanup_d3d_states()
	{
		//cleanup g3d state systems
		g3dRSMRendererDX11::CleanupStates();
		g3dVSMRendererDX11::CleanupStates();
		g3dRenderPass::CleanupStates();
		matTextureMgrDX11::CleanupStates();
		g3dTransparencySortDX11::CleanupStates();
		g3dSceneRendererDX11::CleanupStates();
		g3dRenderScreenSpaceObjects::CleanupStates();
		g3dRenderCameraSpaceObjects::CleanupStates();
		g3dPickBufferRendererDX11::CleanupStates();
		g3dDepthMapRendererDX11::CleanupStates();
		g3dDX11Util::CleanupStates();
		g3dStateMgrDX11::CleanUp();

		SAFE_DELETE( ds_pTest_Write_LessE_NS );
		SAFE_DELETE( st_pInitial );

		//cleaup State Managers last
		g3dRasterizerStateMgr::CleanUp();
		g3dDepthStencilStateMgr::CleanUp();
		g3dBlendStateMgr::CleanUp();
	}

	//------------------------------------------------------------------------
	//  g3dStateResetter
	//------------------------------------------------------------------------
	class g3dStateResetter : public g2dResetHandler
	{
		public:

			virtual void Deallocate();
			virtual void Reallocate();
	};

	void g3dStateResetter::Deallocate()
	{
		cleanup_d3d_states();

		matShaderMgr::DeInitialize();
		effShaderUtilWin::DeInitialize();
	}

	void g3dStateResetter::Reallocate()
	{
		init_d3d_states();

		effShaderUtilWin::Initialize();
		matShaderMgr::Initialize();

		DBG_LOG( "Finished g3dStateResetter::Reallocate" );
	}

	// Init D3D and get video adapter name
	void Init(std::string& o_VideoName)
	{
		DXGI_ADAPTER_DESC desc;
		HRESULT hr = g2dDX11Global::g_pAdapter->GetDesc(&desc);
		DBG_ASSERT(SUCCEEDED(hr), "Could not get video adapter description");
		itString s(desc.Description);
		o_VideoName = itStringUtil::GetStdString(s);
	}
}

//--------------------------------------------------------------------
//  Create and initialize system
//--------------------------------------------------------------------
g3dSystemDX11::g3dSystemDX11()
{
	Init(m_VideoName);

	g2dResetHandler::AddResetHandler( new g3dStateResetter );

	// Setup our texture format info and other stuff
	matD3DGlobal::Initialize();
	g3dDX11Global::Initialize();

	g3dSceneGlobal::g_Identity.Identity();

	init_d3d_states();

	g3dSceneGlobal::g_AdditiveMode = false;
	g3dSceneGlobal::SetTransforms(maPoint3d(0,0,0), maMatrix4x4(), maMatrix4x4());
	g3dSceneGlobal::g_FrameTime = 0.0f;

	g3dSceneGlobal::g_ScreenSpaceViewport = false;

	m_pLightMgrImpl = new g3dLightMgrDX11;
	g3dLightMgr::SetImplementation(m_pLightMgrImpl);
	m_pTextureMgrImpl = new matTextureMgrDX11;
	matTextureMgr::SetImplementation(m_pTextureMgrImpl);
	m_pTextureMgrImpl->Initialize();
	
	effShaderUtilWin::Initialize();
	m_pShaderImpl = new effShaderArray;
	matShaderMgr::SetImplementation(m_pShaderImpl);
	matShaderMgr::Initialize();

	m_pScreenCaptureImpl = new g2dScreenCaptureUtilDX11();
	g2dScreenCaptureUtil::SetImplementation(m_pScreenCaptureImpl);

	g3dDrawStyleUtilDX11::Initialize();

	g2dFontObjectDX11::Init();
}

//--------------------------------------------------------------------
// Clean up and destroy system
//--------------------------------------------------------------------
g3dSystemDX11::~g3dSystemDX11()
{
	g2dFontObjectDX11::Cleanup();

	g2dFontUtil::ReleaseAllFonts();


	g3dDrawStyleUtilDX11::DeInitialize();

	matShaderMgr::DeInitialize();
	effShaderUtilWin::DeInitialize();
	m_pTextureMgrImpl->DeInitialize();

	// delete implementations
	delete m_pLightMgrImpl;
	delete m_pTextureMgrImpl;
	delete m_pShaderImpl;
	delete m_pScreenCaptureImpl;

	cleanup_d3d_states();

	g3dDX11Global::DeInitialize();
	matD3DGlobal::DeInitialize();
}

//------------------------------------------------------------------------
//	GetVideoAdapterName returns some kind of ANSI C string uniquely
//	identifying the type of video hardware in the system.  This function
//	can be called only after calling Init().
//------------------------------------------------------------------------
const char* g3dSystemDX11::GetVideoAdapterName()
{
	return m_VideoName.c_str();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float g3dSystemDX11::GetVideoMemory()
{
	DXGI_ADAPTER_DESC desc;
	HRESULT hr = g2dDX11Global::g_pAdapter->GetDesc(&desc);
	DBG_ASSERT(SUCCEEDED(hr), "Could not get video adapter description");
	return (float)desc.DedicatedVideoMemory;
}




