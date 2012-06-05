/*****************************************************************************
**	gpxShaderParams.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxShaderParams.hpp"

#include "Graphics/Eff/effShaderParams.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxShaderParams::gpxShaderParams(effShaderParams &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	// Copy of shader params to present to the UI and monitor for changes
	m_UIParams.reset( i_Effect.Clone() );

	// Build property objects for this copy - this will be 
	// presented to the GUI thread
	matShaderEffect* eff = m_UIParams->GetShader();
	DBG_ASSERT(eff, "Cannot generate proxy shader interface without shader.");
	if (eff)
	{
		eff->BuildPrtyObject(m_UIParams.get());
	}

	// Add callback on our duplicated shader params to know when an update is needed
	std::vector<effParamTexture*> vTextures;
	m_UIParams->GetAllTextureParams(vTextures);
	for (int i = 0; i < vTextures.size(); i++)
	{	
		vTextures[i]->Property().AddCallback(new prtyCallbackWrapper<gpxShaderParams>(this, &gpxShaderParams::PropertyChanged));
	}

	std::vector<effParamColor*> vColors;
	m_UIParams->GetAllColorParams(vColors);
	for (int i = 0; i < vColors.size(); i++)
	{	
		vColors[i]->Property().AddCallback(new prtyCallbackWrapper<gpxShaderParams>(this, &gpxShaderParams::PropertyChanged));
	}

	std::vector<effParamFloat*> vFloats;
	m_UIParams->GetAllFloatParams(vFloats);
	for (int i = 0; i < vFloats.size(); i++)
	{	
		vFloats[i]->Property().AddCallback(new prtyCallbackWrapper<gpxShaderParams>(this, &gpxShaderParams::PropertyChanged));
	}

	std::vector<effParamBool*> vBools;
	m_UIParams->GetAllBoolParams(vBools);
	for (int i = 0; i < vBools.size(); i++)
	{	
		vBools[i]->Property().AddCallback(new prtyCallbackWrapper<gpxShaderParams>(this, &gpxShaderParams::PropertyChanged));
	}

	std::vector<effParamInt*> vInts;
	m_UIParams->GetAllIntParams(vInts);
	for (int i = 0; i < vInts.size(); i++)
	{	
		vInts[i]->Property().AddCallback(new prtyCallbackWrapper<gpxShaderParams>(this, &gpxShaderParams::PropertyChanged));
	}
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxShaderParams::~gpxShaderParams()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
// Return shader params to use for UI. When proxied,
// these UIInfos do not control the effShaderParams directly, they
// are hooked up to a new set that buffers the changes.
//--------------------------------------------------------------------
effShaderParams& gpxShaderParams::UIShaderParams()
{
#if USE_PROXIES
	return (*m_UIParams);
#else
	return m_Effect;
#endif
}
const effShaderParams& gpxShaderParams::GetUIShaderParams() const
{
#if USE_PROXIES
	return (*m_UIParams);
#else
	return m_Effect;
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxShaderParams::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// This is a convenient safe function that already exists,
	// but it might be more efficient to track a mapping
	// between parameters or track only the parameters that changed.
	m_Effect.SetMatchingParams( this->m_UIParams.get() );

	// Because we have taken the callbacks off of the m_Effect
	// texture parameters (they are only on the UI parameters)
	// we have to assign the textures that we loaded by hand.
	std::vector<effParamTexture*> vTextures;
	m_Effect.GetAllTextureParams(vTextures);
	const int num_textures = vTextures.size();
	for (int i = 0; i < num_textures; i++)
	{
		effParamTexture* curParam = vTextures[i];
		effParamTexture* other = m_UIParams->FindTextureParam(curParam->GetName());
		if (other)
		{
			curParam->SetTexture(other->GetTexture());
			curParam->SetRampTexture(other->GetRampTexture());
			curParam->SetRampProperty(other->GetSmartRampProperty());
		}
	}

	this->SetNeedsUpdate(false);
#endif

	return true;
}

//----------------------------------------------------------------------------
// property callback
//----------------------------------------------------------------------------
void gpxShaderParams::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// When a property in our copy of the shader params changes, then mark
	// that we need to update the original params.
	this->SetNeedsUpdate(true);
}
