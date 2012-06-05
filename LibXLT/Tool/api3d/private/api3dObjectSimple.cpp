/*****************************************************************************
**	api3dObjectSimple.cpp
**
**	Derived class, implements api3dObject for a simple object with
**	a single fragment and single material. It is often used to make icons
**	in the 3D interface.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObjectSimple.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/sc/scObject.hpp"


//--------------------------------------------------------------------
// constructor - this object assumes ownership of the arguments.
//	It will also assign the material to the fragment.
//--------------------------------------------------------------------
api3dObjectSimple::api3dObjectSimple(g3dFragment* i_pFragment, matMaterial *i_pMaterial)
: m_pFragment(i_pFragment), m_pMaterial(i_pMaterial)
{
	DBG_ASSERT(i_pFragment, "Fragment is NULL.");

	// we could allow material to be NULL and get the material from the fragment when needed?
	// but then the shared material would be changing for more than one object, and  this
	// object should be self-contained.
	DBG_ASSERT(i_pMaterial, "Material is NULL.");

	m_pFragment->SetMaterial(i_pMaterial);
	m_pObject = new scObject( m_pFragment );
}


//--------------------------------------------------------------------
// constructor - this object assumes ownership of the arguments.
//	It will also assign the material to the fragment.
//  This variation allows you to apply a transformation to the
//  fragment.
//--------------------------------------------------------------------
api3dObjectSimple::api3dObjectSimple(const maMatrix4x4 &i_Transform, 
								     g3dFragment* i_pFragment, 
									 matMaterial *i_pMaterial)
: m_pFragment(i_pFragment), m_pMaterial(i_pMaterial)
{
	DBG_ASSERT(i_pFragment, "Fragment is NULL.");

	// we could allow material to be NULL and get the material from the fragment when needed?
	// but then the shared material would be changing for more than one object, and  this
	// object should be self-contained.
	DBG_ASSERT(i_pMaterial, "Material is NULL.");

	m_pFragment->SetMaterial(i_pMaterial);

	// Add extra node to hold fragment and its transformation,
	// the root node will be controlled by the scObject and will
	// receive the transformation of the object itself.
	g3dSceneNode *pRootNode = new g3dSceneNode;
	g3dSceneNode *pFragNode = new g3dSceneNode(i_pFragment);
	pFragNode->SetTransform(i_Transform);
	pRootNode->AddChild(pFragNode);
	m_pObject = new scObject( pRootNode );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObjectSimple::~api3dObjectSimple()
{
	delete m_pObject;
	delete m_pFragment;
	delete m_pMaterial;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dObjectSimple::GetPosition() const
{
	return m_pObject->GetPosition();
}
void  api3dObjectSimple::SetPosition(const maPoint3d &i_Position)
{
	m_pObject->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dObjectSimple::GetOrientation() const
{
	return m_pObject->GetOrientation();
}
void  api3dObjectSimple::SetOrientation(const maRotation &i_Rotation)
{
	m_pObject->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dObjectSimple::GetScale() const
{
	return m_pObject->GetScale();
}

//virtual
void  api3dObjectSimple::SetScale(const maVector3d& i_Scale)
{
	m_pObject->SetScale( i_Scale );
}

//virtual
void  api3dObjectSimple::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pObject->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dObjectSimple::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
}
bool api3dObjectSimple::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void api3dObjectSimple::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_pObject->SetActiveInRenderLayer(i_bRenderable);
}
bool api3dObjectSimple::GetActiveInRenderLayer() const
{
	return m_pObject->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void api3dObjectSimple::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_pObject->SetActiveInSceneMgr(i_bRenderable);
}
bool api3dObjectSimple::GetActiveInSceneMgr() const
{
	return m_pObject->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dObjectSimple::GetWorldBox() const
{
	m_pObject->GetBase()->UpdateTotalTransform();
	return m_pObject->GetWorldBox();
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dObjectSimple::SetColor(const maFloatRGBA &i_Color)
{
	effSolidData* pData = dynamic_cast<effSolidData*>(m_pMaterial->GetEffectData());
	if (pData != NULL)
	{
		pData->m_Color = i_Color;
		//pData->m_ColorDiffuse = maFloatRGBA(0,0,0,i_Color.GetAlpha());
		//pData->m_ColorAmbient = maFloatRGBA(0,0,0,1);
		//pData->m_ColorSpecular = maFloatRGBA(0,0,0,1);
		//pData->m_Transparency = i_Color.GetAlpha();
	}
	else
	{
		effTexturedData* pData = dynamic_cast<effTexturedData*>(m_pMaterial->GetEffectData());
		if (pData != NULL)
		{
			pData->m_Color = i_Color;
		}
		else
		{
			DBG_ASSERT(pData != NULL, "api3dobjectsimple::SetColor not using effSolid or effTextured");
		}
	}
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* api3dObjectSimple::GetReference(const char* i_Name)
{
	return NULL;
}

//--------------------------------------------------------------------
// Get list of references
//--------------------------------------------------------------------
void api3dObjectSimple::GetReferenceList(std::vector<std::string> &o_List)
{
	// nothing to do, no named nodes
}


