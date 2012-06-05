/*****************************************************************************
**	api3dParticleGenerator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dParticleGenerator.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scParticleGenerator.hpp"
#include "Graphics/sc/scParticleGeneratorTemplate.hpp"
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
api3dParticleGenerator::api3dParticleGenerator( scParticleGenerator* i_pPG, 
												scParticleGeneratorTemplate* i_pPGT )
:   m_pPG( i_pPG ),
	m_pPGT( i_pPGT ),
	m_bTemplateOwner( true )
{
	if ( i_pPGT == 0 )
	{
		m_bTemplateOwner = false;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dParticleGenerator::~api3dParticleGenerator()
{
	envSTLHelpers::DeleteContainer(m_References);

	delete m_pPG;
	if ( m_bTemplateOwner )
		delete m_pPGT;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dParticleGenerator::GetPosition() const
{
	return m_pPG->GetPosition();
}
void  api3dParticleGenerator::SetPosition(const maPoint3d &i_Position)
{
	m_pPG->SetPosition(i_Position);

	//	update the total transform so it will be correct
	//
	//m_pPG->GetBase()->UpdateTotalTransform();
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dParticleGenerator::GetOrientation() const
{
	return m_pPG->GetOrientation();
}
void  api3dParticleGenerator::SetOrientation(const maRotation &i_Rotation)
{
	m_pPG->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dParticleGenerator::GetScale() const
{
	return m_pPG->GetScale();
}

//virtual
void  api3dParticleGenerator::SetScale(const maVector3d& i_Scale)
{
	m_pPG->SetScale( i_Scale );
}

//virtual
void  api3dParticleGenerator::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pPG->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dParticleGenerator::SetRenderable(bool i_Renderable)
{
	m_pPG->SetRenderable(i_Renderable);
}
bool api3dParticleGenerator::GetRenderable() const
{
	return m_pPG->GetRenderable();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scParticleGenerator* api3dParticleGenerator::GetParticleGenerator()
{
	return m_pPG;
}

//--------------------------------------------------------------------
//	setting this will NOT change ownership flag
//--------------------------------------------------------------------
void api3dParticleGenerator::SetParticleGeneratorTemplate(scParticleGeneratorTemplate* i_pPGT)
{
	m_pPGT = i_pPGT;
}

//--------------------------------------------------------------------
// Accessor to object
//--------------------------------------------------------------------
const scObject* api3dParticleGenerator::GetObject() const 
{
	return m_pPG;
}
scObject* api3dParticleGenerator::Object()
{
	return m_pPG;
}
//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* api3dParticleGenerator::GetReference(const char* i_Name)
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
void api3dParticleGenerator::GetReferenceList(std::vector<std::string> &o_List)
{
	scObject* sc_obj = this->Object();
	g3dSceneNode *node = sc_obj->GetBase();
	get_nodenames(*node, o_List);
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dParticleGenerator::GetWorldBox() const
{
	return m_Bounds;
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dParticleGenerator::SetColor(const maFloatRGBA &i_Color)
{
}
