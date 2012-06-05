/********************************************************************************************\
**  prjltData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRJLT_DATA_HPP
#error prjltData.hpp multiply included
#endif
#define PRJLT_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
//#ifndef PRTY_FILEPATH_HPP
//#include "Core/prty/prtyFilePath.hpp"
//#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3DTRANSFORM_HPP
#include "Core/prty/prtyPoint3dTransform.hpp"
#endif 
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif
#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif

#include <vector>

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class prjltData
{
public:
	//---------------------------------------------------------------------------
	// Multiple variations of shadow-casting lights.
	// Note: the values in this enumeration are used in the file format.
	// If you change this enumeration, you must version and update the file 
	// format also.
	//---------------------------------------------------------------------------
	enum LightType
	{
		e_ProjectedLight = 0,
		e_SpotLight,
		e_DirectionalLight,
		e_NumLightTypes
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltData(const prjltData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~prjltData();

	//-------------------------------------------------
	bool operator == (const prjltData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltData& operator=(const prjltData& i_Data);

public:
	LightType				m_LightType;
	prtyColor				m_Color;
	prtyBoolean				m_Enabled;
	prtyBoolean				m_ShadowSource;
	prtyPoint3d				m_Falloff;
	prtyDistance			m_Range;
	prtyFloat				m_Intensity;
	
	// Note: Direction is now changed to Target
	//prtyVector3d			m_Direction;
	prtyPoint3dTransform	m_Target;
	prtyFloat				m_Angle;
	prtyDistance			m_Scale;
	prtyFloat				m_Aspect;
	prtyFloat				m_Tilt;

	prtyBoolean				m_bDirectional;
	prtyBoolean				m_bConeLighting;
	prtyFloat				m_Penumbra;			

	//prtyFilePath	m_TextureFilename;
	prtyTextureFileName		m_TextureFilename;
	prtyInt32				m_DepthMapSize;

	// params for soft shadow algorithm
	prtyFloat				m_LightSize;
	prtyFloat				m_PCSSAdjust;
	prtyFloat				m_SceneScale;
	prtyEnum				m_ShadowQuality;
	
	prtyFloat				m_ShadowIntensity;
	prtyFloat				m_DepthBias;
	prtyColor				m_ShadowColor;

	prtyBoolean				m_bEditorVisible;
	prtyName				m_Name;
	prtyPoint3dTransform	m_Position;
	prtyRotation			m_Orientation;		// Orientation used by directional lights instead of target

	prtyBoolean				m_bDiffuseEnabled;
	prtyBoolean				m_bSpecularEnabled;

	prtyBoolean				m_bShaftVisible;
	prtyFloat				m_ShaftAlpha;
	prtyFloat				m_ShaftDensity;
	prtyDistance			m_ShaftDistFalloffStart;
	prtyDistance			m_ShaftDistFalloffEnd;
	//prtyFilePath			m_ShaftTextureFilename;
	prtyTextureFileName		m_ShaftTextureFilename;

	prtyBoolean				m_bAffectsFur;
	prtyBoolean				m_bAffectsGlow;

	prtyBoolean		m_bEnabledRamp;
	prtyBoolean		m_bEnabledShaftRamp;
	prtyTrigger		m_RampTrigger;				// not saved
	rmpData			m_RampData;					// Ramp data wrapper
	rmpData			m_ShaftRampData;			// Ramp data for Shaft
	//prtyGradient	m_RampGradient;
	//prtyEnum		m_RampShape;
	//prtyEnum		m_RampInterpolation;
	//prtyTrigger		m_RampTrigger;			
	//prtyInt32		m_RampTexSize;
	//prtyFloat		m_RampUWave;
	//prtyFloat		m_RampVWave;
	//prtyFloat		m_RampNoise;
	//prtyFloat		m_RampNoiseFreq;

	// params for hair shadows
	prtyBoolean				m_HairShadowEnable;
	prtyInt32				m_HairShadowSize;
	prtyEnum				m_HairShadowType;
	prtyFloat				m_HairShadowBias;
	prtyFloat				m_HairShadowDensity;
	prtyFloat				m_HairMinBound;
	prtyFloat				m_HairMaxBound;

	// RSM GI parameters
	prtyBoolean				m_GISource;
	prtyInt32				m_ReflectiveMapSize;

	// Mental Ray parameters
	prtyBoolean				m_bMRayAreaLight;
	prtyEnum				m_MRayAreaLightType;
	prtyInt32				m_MRayAreaLightSampling;
	prtyBoolean				m_bMRayAreaLightVisible;

};

