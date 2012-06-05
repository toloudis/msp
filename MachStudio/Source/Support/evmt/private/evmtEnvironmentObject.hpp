/*****************************************************************************
**	evmtEnvironmentObject.hpp
**
**	An object that is eligible to live in an environment.
**
**	By definition, an object can only belong to one environment. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EVMT_ENVIRONMENTOBJECT_HPP
#error evmtEnvironmentObject.hpp multiply included
#endif
#define EVMT_ENVIRONMENTOBJECT_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class api3dObjectSingle;
class evmtEnvironment;
class maFloatRGBA;
class matTexture;
class nameObject;
class gpxEnvironment;
class swlData;

//============================================================================
// A named 3d object that can be put in an environment
//============================================================================
class evmtObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	evmtObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~evmtObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateName(std::string i_Name);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateDiffuse(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateSpecular(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateSwlData(bool i_bEnable);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	nameString GetName();
	
public:
	nameObject* m_pNameObj;
	evmtEnvironment* m_ContainingEnvironment;

private:
	// use thread-safe proxy instead of object
	//api3dObjectSingle* m_pObject;
	gpxEnvironment* m_pObject;


};


