/*****************************************************************************
**	api3dObjectSingle.cpp
**
**	Abstract Derived class, provides an interface for accessing the
**	internal scObject of derived classes. Most apps shouldn't use this 
**	interface, this is just for the internal api3d interface.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObjectSingle.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObjectSingle::api3dObjectSingle()
: m_pRenderState(NULL), m_pEnvironmentState(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
api3dObjectSingle::~api3dObjectSingle()
{
	if (m_pRenderState) delete m_pRenderState;
	if (m_pEnvironmentState) delete m_pEnvironmentState;
}

//--------------------------------------------------------------------
// Pivot point information - default implementation
//--------------------------------------------------------------------
//virtual 
maPoint3d api3dObjectSingle::GetPivotPoint() const
{
	if (const scObject *pObj = this->GetObject())
	{
		return pObj->GetPivotPoint();
	}
	return maPoint3d(0,0,0);
}
//virtual 
void  api3dObjectSingle::SetPivotPoint(const maPoint3d &i_Position, 
									   bool i_bPreserveTransformation)
{
	if (scObject *pObj = this->Object())
	{
		pObj->SetPivotPoint(i_Position, i_bPreserveTransformation);
	}
}
//virtual 
maVector3d api3dObjectSingle::GetPivotCompensation() const
{
	if (const scObject *pObj = this->GetObject())
	{
		return pObj->GetPivotCompensation();
	}
	return maPoint3d(0,0,0);
}
//virtual 
void  api3dObjectSingle::SetPivotCompensation(const maVector3d &i_Compensation)
{
	if (scObject *pObj = this->Object())
	{
		pObj->SetPivotCompensation(i_Compensation);
	}
}

//--------------------------------------------------------------------
// Get transformation for the root node of the given object.
// This is the matrix for the position, scale, rotation,
// along with pivots stored in this object.
//--------------------------------------------------------------------
//virtual 
void api3dObjectSingle::GetTransformation(maMatrix4x4& o_Transformation)
{
	if (scObject *pObj = this->Object())
	{
		pObj->GetTransformation(o_Transformation);
	}
}

//--------------------------------------------------------------------
//	AddLightToRenderState - add this light to the render state
//	for this object. Creates the g3dRenderState instance if 
//	necessary.
//--------------------------------------------------------------------
void api3dObjectSingle::AddLightToRenderState(g3dLight * i_pLight)
{
	if (!m_pRenderState)
	{
		m_pRenderState = new g3dRenderState();
		this->Object()->GetBase()->SetRenderState(m_pRenderState);
	}
	m_pRenderState->m_Lights.push_back(i_pLight);
}

//--------------------------------------------------------------------
//	RemoveLightFromRenderState
//--------------------------------------------------------------------
void api3dObjectSingle::RemoveLightFromRenderState(g3dLight * i_pLight)
{
	if (m_pRenderState)
	{
		envSTLHelpers::RemoveOneValue(m_pRenderState->m_Lights, i_pLight);
	}
}

//--------------------------------------------------------------------
//	Set diffuse environment map in the render state for this object
//--------------------------------------------------------------------
void api3dObjectSingle::SetRenderStateNameEnv(std::string i_Name)
{
	if (!m_pEnvironmentState)
	{
		m_pEnvironmentState = new g3dAmbientEnvState();
		this->Object()->GetBase()->SetEnvironment(m_pEnvironmentState);
	}
	m_pEnvironmentState->m_Name = i_Name;
}

//--------------------------------------------------------------------
//	Set diffuse environment map in the render state for this object
//--------------------------------------------------------------------
void api3dObjectSingle::SetRenderStateDiffuseEnv(matTexture* i_Map, float i_Weight, float i_Angle,
												  const maFloatRGBA& i_Color)
{
	if (!m_pEnvironmentState)
	{
		m_pEnvironmentState = new g3dAmbientEnvState();
		this->Object()->GetBase()->SetEnvironment(m_pEnvironmentState);
	}
	m_pEnvironmentState->m_DiffuseMap = i_Map;
	m_pEnvironmentState->m_DiffuseFactor = i_Weight;
	m_pEnvironmentState->m_DiffuseAngle = i_Angle;
	m_pEnvironmentState->m_DiffuseColor = i_Color;
}

//--------------------------------------------------------------------
//	Set specular environment map in the render state for this object
//--------------------------------------------------------------------
void api3dObjectSingle::SetRenderStateSpecularEnv(matTexture* i_Map, float i_Weight, float i_Angle,
												  const maFloatRGBA& i_Color)
{
	if (!m_pEnvironmentState)
	{
		m_pEnvironmentState = new g3dAmbientEnvState();
		this->Object()->GetBase()->SetEnvironment(m_pEnvironmentState);
	}
	m_pEnvironmentState->m_SpecularMap = i_Map;
	m_pEnvironmentState->m_SpecularFactor = i_Weight;
	m_pEnvironmentState->m_SpecularAngle = i_Angle;
	m_pEnvironmentState->m_SpecularColor = i_Color;
}

//--------------------------------------------------------------------
//	Set swl data
//--------------------------------------------------------------------
void api3dObjectSingle::SetSwlData(bool i_bEnable)
{
	if (!m_pEnvironmentState)
	{
		m_pEnvironmentState = new g3dAmbientEnvState();
		this->Object()->GetBase()->SetEnvironment(m_pEnvironmentState);
	}

	m_pEnvironmentState->m_bEnableSwlEnv = i_bEnable;
}

//--------------------------------------------------------------------
//	Wireframe sets whether the object drawn in wireframe
//--------------------------------------------------------------------
void api3dObjectSingle::SetWireframe(bool i_Wireframe)
{
	g3dSceneNode::DrawStyle style = (i_Wireframe) ? g3dSceneNode::e_LitWireframe : g3dSceneNode::e_Inherit;
	this->Object()->GetBase()->SetDrawStyle(style);
}
bool api3dObjectSingle::GetWireframe() const
{
	return (this->GetObject()->GetBase()->GetDrawStyle() == g3dSceneNode::e_LitWireframe);
}

//--------------------------------------------------------------------
//	LowResolution sets whether the object should render its
//		low resolution model.  Calls to SetLowResolution may
//		not change anything if there is no low-res model to choose.
//--------------------------------------------------------------------
void api3dObjectSingle::SetLowResolution(bool i_LowResolution)
{
	if (this->GetObject()->HasLowResolutionModel())
		this->Object()->GetBase()->SetForceLowResolution(i_LowResolution);
}
bool api3dObjectSingle::GetLowResolution() const
{
	return (this->GetObject()->GetBase()->GetForceLowResolution());
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the object should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void api3dObjectSingle::SetGPUPickable(bool i_Pickable)
{
	this->Object()->GetBase()->SetGPUPickable(i_Pickable);
}
bool api3dObjectSingle::GetGPUPickable() const
{
	return (this->GetObject()->GetBase()->GetGPUPickable());
}

//--------------------------------------------------------------------
//	PickHull sets whether the object should be rendered in 
//	pick renders even if the Renderable flag is false.
//--------------------------------------------------------------------
void api3dObjectSingle::SetPickHull(bool i_PickHull)
{
	this->Object()->GetBase()->SetPickHull(i_PickHull);
}
bool api3dObjectSingle::GetPickHull() const
{
	return (this->GetObject()->GetBase()->GetPickHull());
}

//----------------------------------------------------------------------------
// PickMask is a user defined bit mask that can be used to filter
// the pickable objects. The default value of "0" means "do not filter".
//----------------------------------------------------------------------------
void api3dObjectSingle::SetPickMask(envType::UInt32 i_PickMask)
{
	this->Object()->GetBase()->SetPickMask(i_PickMask);
}

//--------------------------------------------------------------------
// Inherits Transform controls if the given object uses the 
// transformations of its parent nodes.
//--------------------------------------------------------------------
void api3dObjectSingle::SetInheritsTransform(bool i_bInherits)
{
	// Note have to invert the flag between "inherits" and "ignore"
	this->Object()->GetBase()->SetIgnoreParentTransform( !i_bInherits );
}
bool api3dObjectSingle::GetInheritsTransform() const
{
	// Note have to invert the flag between "inherits" and "ignore"
	return (!this->GetObject()->GetBase()->GetIgnoreParentTransform());
}

//----------------------------------------------------------------------------
// Is the pick code given within the high and low pick codes assigned
// to this node?
//----------------------------------------------------------------------------
bool api3dObjectSingle::ContainsPickCode(envType::UInt32 i_PickCode) const
{
	return (this->GetObject()->GetBase()->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
//	Returns the node with a fragment with the given pick code
//--------------------------------------------------------------------
const g3dSceneNode* api3dObjectSingle::GetPickedNode(envType::UInt32 i_PickCode) const
{
	return (this->GetObject()->GetBase()->GetPickedNode(i_PickCode));
}

//--------------------------------------------------------------------
//	Return the ambient state
//--------------------------------------------------------------------
g3dAmbientEnvState * api3dObjectSingle::GetAmbientState()
{
	return m_pEnvironmentState;
}
