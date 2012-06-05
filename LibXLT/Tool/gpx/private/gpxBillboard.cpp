/*****************************************************************************
**	gpxBillboard.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxBillboard.hpp"

#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxBillboard::gpxBillboard(api3dBillboard &i_Billboard)
:	gpxSceneObject(i_Billboard),
	m_Billboard(i_Billboard)
{
#if USE_PROXIES
	m_pTexture = i_Billboard.GetTexture();
	m_bAdditiveMaterial = i_Billboard.GetAdditiveMaterial();
	m_bOrientToCamera = i_Billboard.GetOrientToCamera();
	m_bSnapToCamera = i_Billboard.GetSnapToCamera();
	m_DistToCamera = i_Billboard.GetDistToCamera();
	m_Camera.reset();

	m_bCKActive = false;;
	m_CKColor.Set(0,0,0,0);
	m_CKTolerance = 0.1f;;
	m_bCKRemoveSpill = false;
	m_CKSpillType = 0;
	m_CKSpillBias = 0.0f;;
	m_CKEdgeBlur = 0;

	m_Brightness = 1.0f;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxBillboard::~gpxBillboard()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxBillboard::SetTexture(matTexture *i_pTexture)
{
	PROXY_SET_OR_STORE(m_Billboard, SetTexture, m_pTexture, i_pTexture);
}
void gpxBillboard::SetAdditiveMaterial(bool i_bAdditive)
{
	PROXY_SET_OR_STORE(m_Billboard, SetAdditiveMaterial, m_bAdditiveMaterial, i_bAdditive);
}
void gpxBillboard::SetOrientToCamera(bool i_bOrient)
{
	PROXY_SET_OR_STORE(m_Billboard, SetOrientToCamera, m_bOrientToCamera, i_bOrient);
}
void gpxBillboard::SetSnapToCamera(bool i_bSnap)
{
	PROXY_SET_OR_STORE(m_Billboard, SetSnapToCamera, m_bSnapToCamera, i_bSnap);
}
void gpxBillboard::SetDistToCamera(float i_Value)
{
	PROXY_SET_OR_STORE(m_Billboard, SetDistToCamera, m_DistToCamera, i_Value);
}
void gpxBillboard::SetCamera(shared_ptr<camCamera> i_Camera)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCamera, m_Camera, i_Camera);
}
void gpxBillboard::SetBrightness(float i_Brightness)
{
	PROXY_SET_OR_STORE(m_Billboard, SetBrightness, m_Brightness, i_Brightness);
}
void gpxBillboard::SetCKActive(bool i_bIsActive)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKActive, m_bCKActive, i_bIsActive);
}
void gpxBillboard::SetCKColor(const maFloatRGBA &i_Color)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKColor, m_CKColor, i_Color);
}
void gpxBillboard::SetCKTolerance(float i_Value)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKTolerance, m_CKTolerance, i_Value);
}
void gpxBillboard::SetCKRemoveSpill(bool i_bIsActive)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKRemoveSpill, m_bCKRemoveSpill, i_bIsActive);
}
void gpxBillboard::SetCKSpillType(int i_Type)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKSpillType, m_CKSpillType, i_Type);
}
void gpxBillboard::SetCKSpillBias(float i_Value)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKSpillBias, m_CKSpillBias, i_Value);
}
void gpxBillboard::SetCKEdgeBlur(int i_Width)
{
	PROXY_SET_OR_STORE(m_Billboard, SetCKEdgeBlur, m_CKEdgeBlur, i_Width);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxBillboard::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// Base class updates position and color information
	gpxSceneObject::Update();

	m_Billboard.SetTexture(m_pTexture);
	m_Billboard.SetAdditiveMaterial(m_bAdditiveMaterial);
	m_Billboard.SetBrightness(m_Brightness);
	m_Billboard.SetOrientToCamera(m_bOrientToCamera);
	m_Billboard.SetSnapToCamera(m_bSnapToCamera);
	m_Billboard.SetDistToCamera(m_DistToCamera);
	m_Billboard.SetCamera(m_Camera);

	m_Billboard.SetCKActive(m_bCKActive);
	m_Billboard.SetCKColor(m_CKColor);
	m_Billboard.SetCKTolerance(m_CKTolerance);
	m_Billboard.SetCKRemoveSpill(m_bCKRemoveSpill);
	m_Billboard.SetCKSpillType(m_CKSpillType);
	m_Billboard.SetCKSpillBias(m_CKSpillBias);
	m_Billboard.SetCKEdgeBlur(m_CKEdgeBlur);
	

	this->SetNeedsUpdate(false);
#endif

	return true;
}
