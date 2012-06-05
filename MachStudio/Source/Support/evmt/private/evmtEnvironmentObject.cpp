/*****************************************************************************
**	evmtEnvironmentObject.cpp
**
**	An object that is eligible to live in an environment.
**
**	By definition, an object can only belong to one environment. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/evmt/private/evmtEnvironmentObject.hpp"

#include "Core/name/nameObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/gpx/gpxEnvironment.hpp"

namespace
{
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
evmtObject::evmtObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject)
:	m_pNameObj(i_pNameObj), 
	m_pObject(new gpxEnvironment(*i_pObject)), // create proxy to object
	m_ContainingEnvironment(NULL) 
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
evmtObject::~evmtObject()
{
	delete m_pObject;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtObject::UpdateName(std::string i_Name)
{
	m_pObject->SetRenderStateNameEnv(i_Name);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtObject::UpdateDiffuse(matTexture* i_Map, float i_Weight, float i_Angle,
										  const maFloatRGBA& i_Color)
{
	m_pObject->SetRenderStateDiffuseEnv(i_Map, i_Weight, i_Angle, i_Color);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtObject::UpdateSpecular(matTexture* i_Map, float i_Weight, float i_Angle,
										  const maFloatRGBA& i_Color)
{
	m_pObject->SetRenderStateSpecularEnv(i_Map, i_Weight, i_Angle, i_Color);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtObject::UpdateSwlData(bool i_bEnable)
{
	m_pObject->SetSwlData(i_bEnable);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
nameString evmtObject::GetName()
{
	return m_pNameObj->GetName();
}
