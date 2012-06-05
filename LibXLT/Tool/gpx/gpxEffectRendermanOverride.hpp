/*****************************************************************************
**	gpxEffectRendermanOverride.hpp
**
**	This class is a thread-safe proxy for a effRendermanOverrideData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTRENDERMANOVERRIDE_HPP
#error gpxEffectRendermanOverride.hpp multiply included
#endif
#define GPX_EFFECTRENDERMANOVERRIDE_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effRendermanOverrideData;

#include <string>


//============================================================================
//============================================================================
class gpxEffectRendermanOverride : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectRendermanOverride(effRendermanOverrideData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectRendermanOverride();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetParamList(std::string i_ParamList);

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetAttributeList(std::string i_AttributeList);

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetOverrideShader(bool i_Val);

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetOverrideAttributes(bool i_Val);

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the light when "Update()" is called.
	//--------------------------------------------------------------------
	void SetShaderLoc(fsLocator i_Loc);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effRendermanOverrideData &m_Effect;

#if USE_PROXIES
	fsLocator m_ShaderLocation;
	std::string m_ParamList;
	std::string m_AttributeList;
	bool m_bOverrideShader;
	bool m_bOverrideAttributes;
#endif
};
