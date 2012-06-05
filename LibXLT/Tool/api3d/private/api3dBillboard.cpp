/*****************************************************************************
**	api3dBillboard.cpp
**
**	Derived class, implements api3dObject with a textured polygon that
**	always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dBillboard.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/eff/effBillboardData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scBillboard.hpp"


//============================================================================
//============================================================================
namespace
{
	maAxisBox l_SharedBox;
}


//--------------------------------------------------------------------
// constructor - The billboard adds a reference to the texture
// and releases in in the destructor
//--------------------------------------------------------------------
api3dBillboard::api3dBillboard(matTexture *i_pTexture)
: m_pObject(NULL), m_pTexture(i_pTexture)
{
	m_pObject = new scBillboard();
	matMaterial* pMat = &m_pObject->GetMaterial();
	pMat->SetAdditive(true);

	// Add texture to billboard
	if (m_pTexture)
	{
		matTextureMgr::IncrementReference(m_pTexture);
		pMat->TypedData<effTexturedData>()->m_Texture = i_pTexture;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dBillboard::~api3dBillboard()
{
	delete m_pObject;
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dBillboard::GetPosition() const
{
	return m_pObject->GetPosition();
}
void  api3dBillboard::SetPosition(const maPoint3d &i_Position)
{
	m_pObject->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dBillboard::GetOrientation() const
{
	return m_pObject->GetOrientation();
}
void  api3dBillboard::SetOrientation(const maRotation &i_Rotation)
{
	m_pObject->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dBillboard::GetScale() const
{
	return m_pObject->GetScale();
}

//virtual
void  api3dBillboard::SetScale(const maVector3d& i_Scale)
{
	m_pObject->SetScale( i_Scale );
}

//virtual
void  api3dBillboard::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pObject->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dBillboard::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
}
bool api3dBillboard::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void api3dBillboard::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_pObject->SetActiveInRenderLayer(i_bRenderable);
}
bool api3dBillboard::GetActiveInRenderLayer() const
{
	return m_pObject->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	ActiveInSceneMgr sets whether the object is visible
//	in scene manager
//--------------------------------------------------------------------
void api3dBillboard::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_pObject->SetActiveInSceneMgr(i_bRenderable);
}
bool api3dBillboard::GetActiveInSceneMgr() const
{
	return m_pObject->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
// If OrientToCamera is true (the default) the billboard will
// rotate to face the camera.
//--------------------------------------------------------------------
bool api3dBillboard::GetOrientToCamera() const
{
	return m_pObject->GetOrientToCamera();
}
void api3dBillboard::SetOrientToCamera(bool i_bOrient)
{
	m_pObject->SetOrientToCamera(i_bOrient);
}

//--------------------------------------------------------------------
// If SnapToCamera is true the billboard will snap to camera view
// and scale to the camera aspect
//--------------------------------------------------------------------
bool api3dBillboard::GetSnapToCamera() const
{
	return m_pObject->GetSnapToCamera();
}
void api3dBillboard::SetSnapToCamera(bool i_bSnap)
{
	m_pObject->SetSnapToCamera(i_bSnap);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void api3dBillboard::SetBrightness(float i_Brightness)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_Brightness = (i_Brightness);

	//m_pObject->SetBrightness(i_Brightness);
}

//--------------------------------------------------------------------
// Set/Get billboard distance to camera
//--------------------------------------------------------------------
float api3dBillboard::GetDistToCamera() const
{
	return m_pObject->GetDistToCamera();
}
void api3dBillboard::SetDistToCamera(float i_Dist)
{
	m_pObject->SetDistToCamera(i_Dist);
}

//--------------------------------------------------------------------
// Set camera to attach on
//--------------------------------------------------------------------
void api3dBillboard::SetCamera(shared_ptr<camCamera> i_Camera)
{
	m_pObject->SetCamera(i_Camera);
}

//--------------------------------------------------------------------
// Set/Get additive flag on billboard's material
//--------------------------------------------------------------------
bool api3dBillboard::GetAdditiveMaterial() const
{
	return m_pObject->GetMaterial().GetAdditive();
}
void api3dBillboard::SetAdditiveMaterial(bool i_bAdditive)
{
	m_pObject->GetMaterial().SetAdditive(i_bAdditive);
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dBillboard::GetWorldBox() const
{
	maPoint3d pos = m_pObject->GetPosition();
	l_SharedBox = maAxisBox(pos, pos);
	l_SharedBox.Swell(0.1f);
	return l_SharedBox;
}


//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dBillboard::SetColor(const maFloatRGBA &i_Color)
{
	effTexturedData* pData = dynamic_cast<effTexturedData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effTextured");

	// Set alpha channel of diffuse in order to allow setting of transparency
	pData->m_Color = (i_Color);
}

//--------------------------------------------------------------------
// Set is chroma key active
//--------------------------------------------------------------------
void api3dBillboard::SetCKActive(bool i_bIsActive)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_bCKActive = (i_bIsActive);
}

//--------------------------------------------------------------------
// Set chroma key color
//--------------------------------------------------------------------
void api3dBillboard::SetCKColor(const maFloatRGBA &i_Color)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_CKColor = (i_Color);
}

//--------------------------------------------------------------------
// Set chroma key tolerance
//--------------------------------------------------------------------
void api3dBillboard::SetCKTolerance(float i_Value)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_CKTolerance = (i_Value);
}

//--------------------------------------------------------------------
// Set is removing spill 
//--------------------------------------------------------------------
void api3dBillboard::SetCKRemoveSpill(bool i_bIsActive)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_bCKRemoveSpill = (i_bIsActive);
}

//--------------------------------------------------------------------
// Set removing spill type
//--------------------------------------------------------------------
void api3dBillboard::SetCKSpillType(int i_Type)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_CKSpillType = (i_Type);
}

//--------------------------------------------------------------------
// Set removing spill bias
//--------------------------------------------------------------------
void api3dBillboard::SetCKSpillBias(float i_Value)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_CKSpillBias = (i_Value);
}

//--------------------------------------------------------------------
// Set chroma key edge blurring width
//--------------------------------------------------------------------
void api3dBillboard::SetCKEdgeBlur(int i_Width)
{
	effBillboardData* pData = dynamic_cast<effBillboardData*>(m_pObject->GetMaterial().GetEffectData());
	DBG_ASSERT(pData != NULL, "api3dBillboard::SetColor not using effBillboardData");

	pData->m_CKEdgeBlur = (i_Width);
}

//--------------------------------------------------------------------
// Set texture for billboard. The billboard will increment the
// reference on the texture and will release the old texture.
//--------------------------------------------------------------------
void api3dBillboard::SetTexture(matTexture *i_pTexture)
{
	if (i_pTexture != m_pTexture)
	{
		matMaterial* pMat = &m_pObject->GetMaterial();

		// Remove old texture
		pMat->RemoveTextures();
		pMat->TypedData<effTexturedData>()->m_Texture = NULL;

		if (m_pTexture)
		{
			matTextureMgr::ReleaseTexture(m_pTexture);
			m_pTexture = NULL;
		}

		if (i_pTexture)
		{
			m_pTexture = i_pTexture;
			matTextureMgr::IncrementReference(m_pTexture);
			pMat->TypedData<effTexturedData>()->m_Texture = m_pTexture;
		}
	}
}

//--------------------------------------------------------------------
// Return pointer to texture for billboard. 
//--------------------------------------------------------------------
matTexture* api3dBillboard::GetTexture() const
{
	return m_pTexture;
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* api3dBillboard::GetReference(const char* i_Name)
{
	return NULL;
}

//--------------------------------------------------------------------
// Get list of references
//--------------------------------------------------------------------
void api3dBillboard::GetReferenceList(std::vector<std::string> &o_List)
{
}

//----------------------------------------------------------------------------
//	get a pointer to the scene object
//----------------------------------------------------------------------------
const scObject* api3dBillboard::GetObject() const
{
	return m_pObject;
}
scObject* api3dBillboard::Object()
{
	return m_pObject;
}



