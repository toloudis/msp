/*****************************************************************************
**  effShaderParams.hpp
**
**      effShaderParams is a collection of prty wrappers representing named
**	shader parameters
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_SHADERPARAMS_HPP
#error effShaderParams.hpp multiply included
#endif
#define EFF_SHADERPARAMS_HPP

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif 
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif 
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef MAT_TEXTURETYPE_HPP
#include "Graphics/Mat/matTextureType.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class prtyTextureFileChooserUIInfo;
class prtyTextureFileName;

//============================================================================
// abstract class to collect all shader param bindings and apply them at once
//============================================================================
class effShaderBindings
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effShaderBindings() {}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~effShaderBindings() {}

	//------------------------------------------------------------------------
	// actually set all variables to shader.
	//------------------------------------------------------------------------
	virtual void Bind() = 0;
};


//============================================================================
// an abstract shader parameter
//============================================================================
class effShaderParam
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effShaderParam(const std::string& i_Name)
		:	m_Name(i_Name)
	{}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~effShaderParam() {}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effShaderParam* Clone() = 0;

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() = 0;

	//------------------------------------------------------------------------
	// get the name of the parameter
	//------------------------------------------------------------------------
	const std::string& GetName() const {return m_Name;}

protected:
	std::string m_Name;
};

//============================================================================
// a float shader parameter
//============================================================================
class effParamFloat : public effShaderParam
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effParamFloat(const std::string& i_Name, const std::string& i_PrtyName, float i_Default)
		: effShaderParam(i_Name), m_Property(i_PrtyName, i_Default)
	{
	}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effParamFloat* Clone();

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() {return &m_Property;}
	prtyFloat& Property() {return m_Property;}
	const prtyFloat& GetProperty() const {return m_Property;}

protected:
	prtyFloat m_Property;
};

//============================================================================
// a int shader parameter
//============================================================================
class effParamInt : public effShaderParam
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effParamInt(const std::string& i_Name, const std::string& i_PrtyName, int i_Default)
		: effShaderParam(i_Name), m_Property(i_PrtyName, i_Default)
	{
	}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effParamInt* Clone();

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() {return &m_Property;}
	prtyInt32& Property() {return m_Property;}
	const prtyInt32& GetProperty() const {return m_Property;}

protected:
	prtyInt32 m_Property;
};

//============================================================================
// a boolean shader parameter
//============================================================================
class effParamBool : public effShaderParam
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effParamBool(const std::string& i_Name, const std::string& i_PrtyName, bool i_Default)
		: effShaderParam(i_Name), m_Property(i_PrtyName, i_Default)
	{
	}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effParamBool* Clone();

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() {return &m_Property;}
	prtyBoolean& Property() {return m_Property;}
	const prtyBoolean& GetProperty() const {return m_Property;}

protected:
	prtyBoolean m_Property;
};

//============================================================================
// a color shader parameter
//============================================================================
class effParamColor : public effShaderParam
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effParamColor(const std::string& i_Name, const std::string& i_PrtyName, const maFloatRGBA& i_Default)
		: effShaderParam(i_Name), m_Property(i_PrtyName, i_Default)
	{
	}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effParamColor* Clone();

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() {return &m_Property;}
	prtyColor& Property() {return m_Property;}
	const prtyColor& GetProperty() const {return m_Property;}

protected:
	prtyColor m_Property;
};

//============================================================================
// a texture shader parameter
//============================================================================
class effParamTexture : public effShaderParam
{
public:
	//------------------------------------------------------------------------
	// ParamTextures should use fsLocator as the value now.
	//------------------------------------------------------------------------
	effParamTexture(const std::string& i_Name, const std::string& i_PrtyName, const fsLocator& i_Default)
		: effShaderParam(i_Name), m_Property(i_PrtyName, i_Default), m_Texture(NULL), m_pRampTexture(NULL), m_Type( TEXTURE_TYPE_UNKNOWN )
	{
	}

	//------------------------------------------------------------------------
	// To support old code, this constructor is kept around in order
	// to check for empty strings in the itString, which cannot
	// be Push'ed onto the fsLocator.
	//------------------------------------------------------------------------
	effParamTexture(const std::string& i_Name, const std::string& i_PrtyName, const itString& i_Default);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effParamTexture(const std::string& i_Name, const std::string& i_PrtyName, matTexture* i_Default, TEXTURE_TYPE i_Type )
		: effShaderParam(i_Name), m_Property(i_PrtyName), m_Texture(i_Default), m_pRampTexture(NULL), m_Type( i_Type )
	{
	}

	//------------------------------------------------------------------------
	// abstract copy 
	//------------------------------------------------------------------------
	virtual effParamTexture* Clone();

	//------------------------------------------------------------------------
	// property access
	//------------------------------------------------------------------------
	virtual prtyProperty* GetBaseProperty() {return &m_Property;}
	//prtyFilePath& Property() {return m_Property;}
	prtyTextureFileName& Property() {return m_Property;}
	const prtyTextureFileName& GetProperty() const {return m_Property;}

	//------------------------------------------------------------------------
	// texture access
	//------------------------------------------------------------------------
	matTexture* GetTexture() const {return m_Texture;}

	//------------------------------------------------------------------------
	// override the filename prty and insert externally specified texture 
	//------------------------------------------------------------------------
	void SetTexture(matTexture* i_pTexture) {m_Texture = i_pTexture;}

	//------------------------------------------------------------------------
	// invalidate texture ptr
	//------------------------------------------------------------------------
	void ClearTexture() {m_Texture = NULL;}

	//------------------------------------------------------------------------
	// texture access
	//------------------------------------------------------------------------
	matTexture* GetRampTexture() const {return m_pRampTexture;}

	//------------------------------------------------------------------------
	// override the filename prty and insert externally specified texture 
	//------------------------------------------------------------------------
	void SetRampTexture(matTexture* i_pTexture) {m_pRampTexture = i_pTexture;}

	//------------------------------------------------------------------------
	// invalidate texture ptr
	//------------------------------------------------------------------------
	void ClearRampTexture() {m_pRampTexture = NULL;}

	//------------------------------------------------------------------------
	// ramp texture access
	//------------------------------------------------------------------------
	prtyObject* GetRampProperty() {return m_RampObject.get();}

	//------------------------------------------------------------------------
	// ramp texture access
	//------------------------------------------------------------------------
	shared_ptr<prtyObject> GetSmartRampProperty() {return m_RampObject;}

	//------------------------------------------------------------------------
	// insert externally specified ramp texture 
	//------------------------------------------------------------------------
	void SetRampProperty(std::auto_ptr<prtyObject> i_RampObject) { m_RampObject = i_RampObject; }
	void SetRampProperty(shared_ptr<prtyObject> i_RampObject) { m_RampObject = i_RampObject; }

	//------------------------------------------------------------------------
	// clear any ramp data
	//------------------------------------------------------------------------
	void ClearRampProperty(){ m_RampObject.reset(); }

	//------------------------------------------------------------------------
	// load texture based on prty value, updating restore name if successful
	//------------------------------------------------------------------------
	bool Load(const fsResourceFinder& i_Finder);

	//------------------------------------------------------------------------
	// maintain local copy of texture name in case prty change fails
	//------------------------------------------------------------------------
	const fsLocator& GetRestoreTextureName() const {return m_TextureName;}
	bool IsNameCurrent() const {return (m_Property.GetValue() == m_TextureName);}

	// Gets the type of texture this parameter expects
	void SetType( TEXTURE_TYPE i_Type ){ m_Type = i_Type; }
	TEXTURE_TYPE GetType() {return m_Type;}

	//------------------------------------------------------------------------
	// If the shader had a filename to use as a default, store that
	// filename here.
	//------------------------------------------------------------------------
	const fsLocator& GetShaderResourceName() const {return m_ShaderResourceName;}
	void SetShaderResourceName(const fsLocator& i_Name) { m_ShaderResourceName = i_Name;}

protected:
	//prtyFilePath m_Property;
	prtyTextureFileName m_Property;

	fsLocator m_TextureName;
	matTexture* m_Texture;
	matTexture* m_pRampTexture;
	shared_ptr<prtyObject> m_RampObject;
	TEXTURE_TYPE m_Type;
	fsLocator m_ShaderResourceName;
};

//============================================================================
// a collection of shader parameters
//============================================================================
class effShaderParams
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	effShaderParams();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~effShaderParams();

	//------------------------------------------------------------------------
	// abstract copy
	//------------------------------------------------------------------------
	virtual effShaderParams* Clone() const;

	//------------------------------------------------------------------------
	// shader access
	//------------------------------------------------------------------------
	void SetShaderName(const fsLocator& i_ShaderPath, matShaderEffect* i_pShader = NULL);
	fsLocator GetShaderName() {return m_ShaderName;}
	matShaderEffect* GetShader();

	//------------------------------------------------------------------------
	// add parameters
	//------------------------------------------------------------------------
	void AddParam(effParamTexture* i_Param);
	void AddParam(effParamColor* i_Param);
	void AddParam(effParamFloat* i_Param);
	void AddParam(effParamBool* i_Param);
	void AddParam(effParamInt* i_Param);

	//------------------------------------------------------------------------
	// get parameters in bulk
	//------------------------------------------------------------------------
	void GetAllTextureParams(std::vector<effParamTexture*>& o_TextureParams) const;
	void GetAllColorParams(std::vector<effParamColor*>& o_ColorParams) const;
	void GetAllFloatParams(std::vector<effParamFloat*>& o_FloatParams) const;
	void GetAllBoolParams(std::vector<effParamBool*>& o_BoolParams) const;
	void GetAllIntParams(std::vector<effParamInt*>& o_IntParams) const;

	//------------------------------------------------------------------------
	// find parameters by name and type
	//------------------------------------------------------------------------
	effShaderParam* FindParam(const std::string& i_Name) const;
	effParamTexture* FindTextureParam(const std::string& i_Name) const;
	effParamColor* FindColorParam(const std::string& i_Name) const;
	effParamFloat* FindFloatParam(const std::string& i_Name) const;
	effParamBool* FindBoolParam(const std::string& i_Name) const;
	effParamInt* FindIntParam(const std::string& i_Name) const;

	//------------------------------------------------------------------------
	// load all textures in a loop
	//------------------------------------------------------------------------
	void ReloadTextures(const fsResourceFinder& i_pFinder) const;

	//------------------------------------------------------------------------
	// set all textures to null
	//------------------------------------------------------------------------
	void RemoveTextures() const;

	//------------------------------------------------------------------------
	// get a list of all non null textures
	//------------------------------------------------------------------------
	void GetTextures(std::vector<matTexture*>& o_Textures);

	//------------------------------------------------------------------------
	// debug output
	//------------------------------------------------------------------------
	void Dump();

	//------------------------------------------------------------------------
	// is this considered a transparent material (alpha < 1 for any pixel)
	//------------------------------------------------------------------------
	bool HasTransparency();

	//------------------------------------------------------------------------
	// set default values by matching name and type
	//------------------------------------------------------------------------
	void SetMatchingParams(const effShaderParams* i_pOtherParams);

	//------------------------------------------------------------------------
	// shader version
	//------------------------------------------------------------------------
	int GetVersion() const;
	void SetVersion(int i_Version);

	void Clear();
/*
	// common and default shading parameters
	void SetDiffuse(const maFloatRGBA& i_Color);
	void SetAmbient(const maFloatRGBA& i_Color);
	void SetSpecular(const maFloatRGBA& i_Color);
	void SetEmissive(const maFloatRGBA& i_Color);
	maFloatRGBA GetDiffuse();
	maFloatRGBA GetAmbient();
	maFloatRGBA GetSpecular();
	maFloatRGBA GetEmissive();
	void SetDiffuseTexture(matTexture* i_pTexture);
	matTexture* GetDiffuseTexture();
	matTexture* GetOpacityTexture();
	float GetOpacityValue();
*/
	//////////////////////////////////////////////////////////////////////////
	
	void SetShaderPathObject(prtyTextureFileName* i_Path);

	// can be null if shader not yet loaded
	effShaderBindings* m_pShaderBindings;

	// all the prty ui data for this object lives here.
	prtyObject* m_pPrtyUI;
	prtyTextureFileName* m_pPathObject;

	struct effTextureUI
	{
		effTextureUI(prtyTextureFileChooserUIInfo* i_pUIInfo, effParamTexture* i_pParam)
			: m_pUIInfo(i_pUIInfo), m_pParam(i_pParam) {}
	
		prtyTextureFileChooserUIInfo* m_pUIInfo;
		effParamTexture* m_pParam;
	};
	std::vector<effTextureUI> m_TextureParamUIs;

	// params for obtaining transparency information.
	effParamTexture* m_pDiffuseMap;
	effParamTexture* m_pTransparencyMap;
	effParamColor* m_pDiffuseColor;
	effParamFloat* m_pTransparency;

	// shader id
	fsLocator m_ShaderName;

protected:
	// shader ptr, can be null if shader not yet loaded
	matShaderEffect* m_pShader;

	// all the variables
	std::vector<effParamTexture*> m_TextureParams;
	std::vector<effParamColor*> m_ColorParams;
	std::vector<effParamFloat*> m_FloatParams;
	std::vector<effParamBool*> m_BoolParams;
	std::vector<effParamInt*> m_IntParams;

	int m_Version;
};
