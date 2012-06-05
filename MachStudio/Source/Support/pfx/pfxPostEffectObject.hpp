/********************************************************************************************\
** pfxPostEffectObject.hpp
**
**		Property object for post effect of a render layer
**
**  StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef PFX_POSTEFFECTOBJECT_HPP
#error pfxPostEffectObject.hpp multiply included
#endif
#define PFX_POSTEFFECTOBJECT_HPP

// enable this flag to have rmp texture saving UI
//#define SAVE_RMP_TEXTURE

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PFX_DATA_HPP
#include "Support/pfx/pfxData.hpp"
#endif
#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif 
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif 

class effParamTexture;
class effShaderParams;
class gpxShaderParams;
class prtyPropertyUIInfoContainer;
class prtyObject;

class pfxPostEffectObject : public prtyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//pfxPostEffectObject(pfxData::UIType i_Type = pfxData::e_ViewPort);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	pfxPostEffectObject(const std::string& i_ShaderName, 
						const std::string& i_ShaderPath,
						pfxData::UIType i_Type = pfxData::e_ViewPort);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	pfxPostEffectObject(const pfxData& i_Data, pfxData::UIType i_Type);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~pfxPostEffectObject();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shared_ptr<prtyPropertyUIInfoContainer> GetBaseContainer();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shared_ptr<prtyPropertyUIInfoContainer> GetShaderContainer();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ApplyPostEffect();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool ForceLoadTexture(const fsLocator& i_Path, int i_Index = 0);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(const pfxData& i_Data);
	pfxData& GetData();

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void RemoveProperties(shared_ptr<prtyPropertyUIInfoContainer> i_SubCategory,
						   prtyObject* i_pObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddPropertiesCallback(shared_ptr<effShaderParams> i_pParam);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void RebuildForm();

	//----------------------------------------------------------------------------
	// property callbacks
	//----------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateShader(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateProperties(prtyProperty *i_pProperty, bool i_bDirty);

	//shared_ptr<effShaderParams> m_ShaderParams;

	// UI Container
	shared_ptr<prtyPropertyUIInfoContainer> m_BaseUI;
	shared_ptr<prtyPropertyUIInfoContainer> m_ShaderUI;

	pfxData m_Data;
	prtyEnum		m_UIType;
	effShaderParams* m_pMatchParams;
	//prtyFilePath m_ShaderType;

	// Ramp data attributes
	effParamTexture* m_pRampParam;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetupRampParam();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTexturePrty(prtyProperty *i_pProperty, bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void NotifyRampUI();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RampChangedFromUI(bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateRampTexture(bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ConfirmRampTexture(int i_texSize);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void UpdateTexture(effParamTexture* i_pParam, bool i_bDirty);

	//------------------------------------------------------------------------
	// Load a texture.  Do not change out params if load fails.
	//------------------------------------------------------------------------
	bool LoadTexture(effParamTexture* i_pParam, bool i_bDirty);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void DeleteTextures(effShaderParams* i_pParam);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
#ifdef SAVE_RMP_TEXTURE
	prtyText		m_SaveRampToFile;
	prtyTrigger		m_SaveRampToFileTrigger;
	void SaveToFile(prtyProperty *i_pProperty, bool i_bDirty);
#endif
};

