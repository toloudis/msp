/*****************************************************************************
**	evmtEnvironment.hpp
**
**	Keeps track of objects that can be grouped based on environment maps
**
**	By definition, an object can only belong to one environment. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EVMT_ENVIRONMENT_HPP
#error evmtEnvironment.hpp multiply included
#endif
#define EVMT_ENVIRONMENT_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <vector>

//============================================================================
//	Forward References
//============================================================================
class evmtObject;
class matTexture;
class rmpData;
class swlData;
class gpxGlobalAmbient;

//============================================================================
// An environment
//============================================================================
class evmtEnvironment
{
public:
	nameString m_Name;
	fsLocator m_DiffuseMapName;
	matTexture* m_DiffuseMap;
	float m_DiffuseFactor;
	float m_DiffuseAngle;
	maFloatRGBA m_DiffuseColor;
	fsLocator m_SpecularMapName;
	matTexture* m_SpecularMap;
	float m_SpecularFactor;
	float m_SpecularAngle;
	maFloatRGBA m_SpecularColor;
	matTexture* m_pRampTexture;
	int m_RampTextureSize;
	std::vector<evmtObject*> m_Objects;
	
	const bool m_bIsSwl;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	evmtEnvironment(bool i_bIsSwl = false);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~evmtEnvironment();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetName(const nameString& i_Name);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseFactor(float i_Factor);
	void SetSpecularFactor(float i_Factor);
	float GetDiffuseFactor();
	float GetSpecularFactor();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseAngle(float i_Angle);
	float GetDiffuseAngle();
	void SetSpecularAngle(float i_Angle);
	float GetSpecularAngle();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseMap(const fsLocator& i_FileName);
	void SetSpecularMap(const fsLocator& i_FileName);
	fsLocator GetDiffuseMapName();
	fsLocator GetSpecularMapName();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseTexture(matTexture* i_pTexture);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseColor(const maFloatRGBA& i_Color);
	void SetSpecularColor(const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetData(std::vector<nameString>& o_Objects);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Remove(evmtObject* i_Object);

	//--------------------------------------------------------------------
	//	Remove objects but maintain the linkage
	//--------------------------------------------------------------------
	void RemoveAllObjects();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Disconnect();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Add(evmtObject* i_pObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void RampChanged(rmpData i_RampData);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ConfirmRampTexture(int i_TexSize);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Update();

	//--------------------------------------------------------------------
	// Make current environment setting to global
	//--------------------------------------------------------------------
	void UpdateToSceneGlobal(gpxGlobalAmbient* i_gpxGlobalAmbient,
							const swlData& i_swlData);
};


