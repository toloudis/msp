/*****************************************************************************
**	cptrWatermarkObject.cpp
**
**	3D Object that holds the watermark for rendering
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWatermarkObject.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//const float c_TargetTransparency = 0.3f;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrWatermarkObject::cptrWatermarkObject()
:	m_pTextureFrag(NULL),
	m_pTextureObject(NULL),
	m_pWatermarkTexture(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrWatermarkObject::~cptrWatermarkObject()
{
}

//--------------------------------------------------------------------
//	Create and Add the watermark
//--------------------------------------------------------------------
void cptrWatermarkObject::Initialize(const fsLocator& i_ResourceID)
{
	const bool l_MIPMAP = false;
	m_pWatermarkTexture = matTextureMgr::LoadTexture( i_ResourceID, TEXTURE_TYPE_2D, l_MIPMAP );

	g3dFragment* m_pTextureFrag;
	const bool lc_morphable = false;
	float wmw, wmh;		// watermark dimensions
	wmw = (float)(m_pWatermarkTexture->GetWidth());
	wmh = (float)(m_pWatermarkTexture->GetHeight());
	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
	maPoint2d origpt( (float)wmw, (float)wmh );
	maPoint2d targetpt;
	targetpt.SetX( origpt.GetX() / winsize.GetX() * 1.0f );
	targetpt.SetY( origpt.GetY() / winsize.GetY() * 1.0f );

	m_pTextureFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(targetpt.GetX(), targetpt.GetY(), 1, 1, lc_morphable);
	m_pTextureFrag->SetDoubleSided(true);

	// Material

	matMaterial * pTexMat = new matMaterial("Billboard.fx");
	effTexturedData* pData = dynamic_cast<effTexturedData*>(pTexMat->GetEffectData());
	DBG_ASSERT(pData != NULL, "cptrModeRender not using effTextureData");
	const float lc_TRANSPARENCY = 0.20f;
	pData->m_Color = maFloatRGBA( 1.0f, 1.0f, 1.0f, lc_TRANSPARENCY );
	pData->m_Texture = m_pWatermarkTexture;
	pTexMat->SetHasSpecular( false );

	// Set up fragment in object
	m_pTextureObject = new api3dObjectSimple(m_pTextureFrag, pTexMat);
	m_pTextureObject->SetGPUPickable(false);
	m_pTextureObject->SetPosition(maPoint3d(0,0,0)); // center of the screen
	api3dScene::AddObject(m_pTextureObject, mnmApp::GetScreenSpaceIndex());
	m_pTextureObject->SetRenderable( true ); // not visible until we get a texture
}

//--------------------------------------------------------------------
//	Remove and Destroy the watermark
//--------------------------------------------------------------------
void cptrWatermarkObject::DeInitialize()
{
	if (m_pTextureObject != NULL)
		api3dScene::RemoveObject(m_pTextureObject, mnmApp::GetScreenSpaceIndex());

	matTextureMgr::ReleaseTexture(m_pWatermarkTexture);
	m_pWatermarkTexture = NULL;

	if (m_pTextureObject != NULL)
		delete m_pTextureObject;
}
