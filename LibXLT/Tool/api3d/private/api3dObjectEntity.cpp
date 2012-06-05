/*****************************************************************************
**	api3dObjectEntity.cpp
**
**	Derived class, implements api3dObject through entEntity
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObjectEntity.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/Ent/entModelInstance.hpp"
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
		{
			// Keep list unique
			if (!envSTLHelpers::Contains(o_List, name))
				o_List.push_back(name);
		}

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
api3dObjectEntity::api3dObjectEntity(entModelTemplate *i_Template, 
									 entModelInstance *i_Instance,
									 entEntity *i_Entity)
: m_pEntity(i_Entity), m_pModelInstance(i_Instance), m_pModelTemplate(i_Template)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObjectEntity::~api3dObjectEntity()
{
	envSTLHelpers::DeleteContainer(m_References);

	delete m_pEntity;
	delete m_pModelInstance;

	// Try to release the template, but if the shared mgr didn't have it
	// then we have to delete it ourself.
	if (api3dSharedModelMgr::ReleaseModelTemplate(m_pModelTemplate) == -1)
		delete m_pModelTemplate;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dObjectEntity::GetPosition() const
{
	return m_pEntity->GetPosition();
}
void  api3dObjectEntity::SetPosition(const maPoint3d &i_Position)
{
	m_pEntity->SetPosition(i_Position);
}
//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dObjectEntity::GetOrientation() const
{
	return m_pEntity->GetOrientation();
}
void  api3dObjectEntity::SetOrientation(const maRotation &i_Rotation)
{
	m_pEntity->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dObjectEntity::GetScale() const
{
	return m_pEntity->GetScale();
}

//virtual
void  api3dObjectEntity::SetScale(const maVector3d& i_Scale)
{
	m_pEntity->SetScale( i_Scale );
}

//virtual
void  api3dObjectEntity::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pEntity->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dObjectEntity::SetRenderable(bool i_Renderable)
{
	m_pEntity->SetRenderable(i_Renderable);
}
bool api3dObjectEntity::GetRenderable() const
{
	return m_pEntity->GetRenderable();
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void api3dObjectEntity::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_pEntity->SetActiveInRenderLayer(i_bRenderable);
}
bool api3dObjectEntity::GetActiveInRenderLayer() const
{
	return m_pEntity->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	ActiveInSceneMgr sets whether the object is visible
//	in scene manager
//--------------------------------------------------------------------
void api3dObjectEntity::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_pEntity->SetActiveInSceneMgr(i_bRenderable);
}
bool api3dObjectEntity::GetActiveInSceneMgr() const
{
	return m_pEntity->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dObjectEntity::GetWorldBox() const
{
	//const maAxisBox& box = m_pEntity->GetUpdatedWorldBox();
	//DBG_LOG3( "OEMin( %8.3f, %8.3f, %8.3f )", box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );

	return m_pEntity->GetUpdatedWorldBox();
}

//--------------------------------------------------------------------
// Force through one frame of animation in order to 
// get an updated bounding box
//--------------------------------------------------------------------
void api3dObjectEntity::AnimateSingleFrame()
{
	m_pEntity->Object()->Animate( appSimTime::GetTime() );
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dObjectEntity::SetColor(const maFloatRGBA &i_Color)
{
	int num_mats = m_pModelTemplate->GetMaterials().size();
	for (int i=0; i<num_mats; i++)
	{
		matMaterial *pMat = m_pModelTemplate->Materials()[i];

		effPhongData* pData = dynamic_cast<effPhongData*>(pMat->GetEffectData());
		DBG_ASSERT(pData != NULL, "api3dobjectEntity::SetColor not using effPhong");

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
api3dReference* api3dObjectEntity::GetReference(const char* i_Name)
{
	scObject* sc_obj = this->Object();
	g3dSceneNode *node = sc_obj->GetBase()->GetNamedNode(i_Name);
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
void api3dObjectEntity::GetReferenceList(std::vector<std::string> &o_List)
{
	scObject* sc_obj = this->Object();
	g3dSceneNode *node = sc_obj->GetBase();
	get_nodenames(*node, o_List);
}

//----------------------------------------------------------------------------
//	get a pointer to the scene object
//----------------------------------------------------------------------------
const scObject* api3dObjectEntity::GetObject() const
{
	return m_pEntity->GetObject();
}
scObject* api3dObjectEntity::Object()
{
	return m_pEntity->Object();
}

//----------------------------------------------------------------------------
//	get a pointer to the entity
//----------------------------------------------------------------------------
entEntity* api3dObjectEntity::GetEntity()
{
	return m_pEntity;
}


//----------------------------------------------------------------------------
//	get a pointer to the entity template
//----------------------------------------------------------------------------
entModelInstance* api3dObjectEntity::GetModelInstance()
{
	return m_pModelInstance;
}
entModelTemplate* api3dObjectEntity::GetModelTemplate()
{
	return m_pModelTemplate;
}
//entEntityTemplate* api3dObjectEntity::GetEntityTemplate()
//{
//	return dynamic_cast<entEntityTemplate*>(m_pModelTemplate);
//}

