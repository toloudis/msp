/*****************************************************************************
**  mtrLightMgr.hpp
**
**		Picks with mouse input
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_LIGHTMGR_HPP
#error mtrLightMgr.hpp multiply included
#endif
#define MTR_LIGHTMGR_HPP

#ifndef MA_AXISBOX_HPP
#include "maAxisBox.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "maFloatRGBA.hpp"
#endif


namespace mtrLightMgr
{

	//============================================================================
	//	Initialize()
	//============================================================================
	void		Initialize();

	//============================================================================
	//	DeInitialize()
	//============================================================================
	void		DeInitialize();

	//============================================================================
	//	Center - set center of lighting. point lights will rotate around this
	//============================================================================
	void SetCenter(const maAxisBox& i_Box);

	//============================================================================
	// DirectionalLight properties
	//============================================================================
	maFloatRGBA	GetDirLightColor();
	void SetDirLightColor(const maFloatRGBA& i_Color);

	void GetDirLightDirection(float &o_Pitch, float &o_Yaw);
	void SetDirLightDirection(float i_Pitch, float i_Yaw);

	bool GetDirLightCastShadow();
	void SetDirLightCastShadow(bool i_bCastShadow);

	bool IsEnabledDirLight();
	void EnableDirLight(bool i_bEnabled);

	bool IsEnabledDirLightFur();
	void EnableDirLightFur(bool i_bFur);

	//============================================================================
	// PointLight1 properties
	//============================================================================
	maFloatRGBA	GetPointLight1Color();
	void SetPointLight1Color(const maFloatRGBA& i_Color);

	void GetPointLight1Position(float &o_Pitch, float &o_Yaw, float &o_Radius);
	void SetPointLight1Position(float i_Pitch, float i_Yaw, float i_Radius);

	bool GetPointLight1CastShadow();
	void SetPointLight1CastShadow(bool i_bCastShadow);

	bool IsEnabledPointLight1();
	void EnablePointLight1(bool i_bEnabled);

	bool IsEnabledPointLightFur();
	void EnablePointLightFur(bool i_bFur);

	//============================================================================
	// ProjectedLight properties
	//============================================================================
	maFloatRGBA	GetProjectedLightColor();
	void SetProjectedLightColor(const maFloatRGBA& i_Color);

	void GetProjectedLightPosition(float &o_Pitch, float &o_Yaw, float &o_Radius);
	void SetProjectedLightPosition(float i_Pitch, float i_Yaw, float i_Radius);

	bool IsEnabledProjectedLight();
	void EnableProjectedLight(bool i_bEnabled);

	bool IsEnabledProjectedLightFur();
	void EnableProjectedLightFur(bool i_bFur);
}
