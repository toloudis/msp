/*****************************************************************************
**	api3dObjectGeom.cpp
**
**	Derived class, implements api3dObject through scObject for
**	non-animating geometry
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObjectGeom.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dSharedModelMgr.hpp"
#include "Tool/api3d/private/api3dNodeReference.hpp"


//============================================================================
//============================================================================
namespace
{
	void get_nodenames(g3dSceneNode &i_Node, std::vector<std::string> &o_List)
	{
		std::string name(i_Node.GetName());
		if (!name.empty())
			o_List.push_back(name);

		int num_kids = i_Node.GetNumChildren();
		for (int i=0; i<num_kids; i++)
		{
			get_nodenames(*i_Node.GetChild(i), o_List);
		}
	}
}

//--------------------------------------------------------------------
// constructor - this object assumes ownership of the arguments
//--------------------------------------------------------------------
api3dObjectGeom::api3dObjectGeom(entModelTemplate *i_Template, 
					entModelInstance *i_Instance,
					scObject *i_Object)
: m_pObject(i_Object), m_pModelTemplate(i_Template), m_pModelInstance(i_Instance)
{

}

//--------------------------------------------------------------------
// protected acess for derived classes to create template and
//	object in constructor
//--------------------------------------------------------------------
api3dObjectGeom::api3dObjectGeom()
: m_pObject(NULL), m_pModelTemplate(NULL), m_pModelInstance(NULL)
{
}
void api3dObjectGeom::SetPointers(entModelTemplate *i_Template, scObject *i_Object)
{
	m_pObject = i_Object;
	m_pModelTemplate = i_Template;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObjectGeom::~api3dObjectGeom()
{
	envSTLHelpers::DeleteContainer(m_References);
	delete m_pObject;
	delete m_pModelInstance;

	// Try to release the template, but if the shared mgr didn't have it
	// then we have to delete it ourself.
	if (api3dSharedModelMgr::ReleaseModelTemplate(m_pModelTemplate) == -1)
		delete m_pModelTemplate;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dObjectGeom::GetPosition() const
{
	return m_pObject->GetPosition();
}
void  api3dObjectGeom::SetPosition(const maPoint3d &i_Position)
{
	m_pObject->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dObjectGeom::GetOrientation() const
{
	return m_pObject->GetOrientation();
}
void  api3dObjectGeom::SetOrientation(const maRotation &i_Rotation)
{
	m_pObject->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dObjectGeom::GetScale() const
{
	return m_pObject->GetScale();
}

//virtual
void  api3dObjectGeom::SetScale(const maVector3d& i_Scale)
{
	m_pObject->SetScale( i_Scale );
}

//virtual
void  api3dObjectGeom::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pObject->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dObjectGeom::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
}
bool api3dObjectGeom::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//--------------------------------------------------------------------
//	Set active sets whether the object is visible in render layer
//--------------------------------------------------------------------
void api3dObjectGeom::SetActiveInRenderLayer(bool i_Renderable)
{
	m_pObject->SetActiveInRenderLayer(i_Renderable);
}
bool api3dObjectGeom::GetActiveInRenderLayer() const
{
	return m_pObject->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	Set active sets whether the object is visible in scene mgr
//--------------------------------------------------------------------
void api3dObjectGeom::SetActiveInSceneMgr(bool i_Renderable)
{
	m_pObject->SetActiveInSceneMgr(i_Renderable);
}
bool api3dObjectGeom::GetActiveInSceneMgr() const
{
	return m_pObject->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dObjectGeom::GetWorldBox() const
{
	m_pObject->GetBase()->UpdateTotalTransform();
	return m_pObject->GetWorldBox();
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dObjectGeom::SetColor(const maFloatRGBA &i_Color)
{
	int num_mats = m_pModelTemplate->GetMaterials().size();
	for (int i=0; i<num_mats; i++)
	{
		matMaterial *pMat = m_pModelTemplate->Materials()[i];

		effPhongData* pData = dynamic_cast<effPhongData*>(pMat->GetEffectData());
		DBG_ASSERT(pData != NULL, "api3dobjectgeom::SetColor not using effPhong");

		pData->m_ColorEmissive = (i_Color);
		pData->m_ColorDiffuse = maFloatRGBA(0,0,0,1);
		pData->m_ColorAmbient = maFloatRGBA(0,0,0,1);
		pData->m_ColorSpecular = maFloatRGBA(0,0,0,1);
		pData->m_Transparency = 1;
	}
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* api3dObjectGeom::GetReference(const char* i_Name)
{
	g3dSceneNode *node = m_pObject->GetBase()->GetNamedNode(i_Name);
	if (node)
	{
		api3dReference *ref = new api3dNodeReference(*node);
		m_References.push_back(ref);
		return ref;
	}
	return NULL;
}

//--------------------------------------------------------------------
// Get list of references
//--------------------------------------------------------------------
void api3dObjectGeom::GetReferenceList(std::vector<std::string> &o_List)
{
	g3dSceneNode *node = m_pObject->GetBase();
	get_nodenames(*node, o_List);
}

//----------------------------------------------------------------------------
//	get a pointer to the template
//----------------------------------------------------------------------------
entModelInstance* api3dObjectGeom::GetModelInstance()
{
	return m_pModelInstance;
}
entModelTemplate* api3dObjectGeom::GetModelTemplate()
{
	return m_pModelTemplate;
}

//----------------------------------------------------------------------------
//	get a pointer to the scene object
//----------------------------------------------------------------------------
const scObject* api3dObjectGeom::GetObject() const
{
	return m_pObject;
}
scObject* api3dObjectGeom::Object()
{
	return m_pObject;
}

