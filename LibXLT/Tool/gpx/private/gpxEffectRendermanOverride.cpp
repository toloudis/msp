/*****************************************************************************
**	gpxEffectRendermanOverride.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectRendermanOverride.hpp"

#include "Graphics/Eff/effRendermanOverrideData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectRendermanOverride::gpxEffectRendermanOverride(effRendermanOverrideData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_ParamList = i_Effect.m_ParamList;
	m_AttributeList = i_Effect.m_AttributeList;
	m_ShaderLocation = i_Effect.m_ShaderLocation;
	m_bOverrideShader = i_Effect.m_bOverrideShader;
	m_bOverrideAttributes = i_Effect.m_bOverrideAttributes;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectRendermanOverride::~gpxEffectRendermanOverride()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectRendermanOverride::SetParamList(std::string i_ParamList)
{
	PROXY_SET_VARIABLE(m_Effect.m_ParamList, m_ParamList, i_ParamList);
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectRendermanOverride::SetAttributeList(std::string i_AttributeList)
{
	PROXY_SET_VARIABLE(m_Effect.m_AttributeList, m_AttributeList, i_AttributeList);
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectRendermanOverride::SetOverrideShader(bool i_Val)
{
	PROXY_SET_VARIABLE(m_Effect.m_bOverrideShader, m_bOverrideShader, i_Val);
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectRendermanOverride::SetOverrideAttributes(bool i_Val)
{
	PROXY_SET_VARIABLE(m_Effect.m_bOverrideAttributes, m_bOverrideAttributes, i_Val);
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectRendermanOverride::SetShaderLoc(fsLocator i_Loc)
{
	PROXY_SET_VARIABLE(m_Effect.m_NameShaderLocation, m_ShaderLocation, i_Loc);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectRendermanOverride::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_ShaderLocation = m_ShaderLocation;
	m_Effect.m_ParamList = m_ParamList;
	m_Effect.m_AttributeList = m_AttributeList;
	m_Effect.m_bOverrideShader = m_bOverrideShader;
	m_Effect.m_bOverrideAttributes = m_bOverrideAttributes;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
