/********************************************************************************************\
**  cmraData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef CMRA_DATA_HPP
#error cmraData.hpp multiply included
#endif
#define CMRA_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

struct cmraCameraChangeData
{
	cmraCameraChangeData::cmraCameraChangeData() : m_UpdateFlag( 0 ){}

	enum CHANGEFLAGS
	{
		UPDATE_POS = 1<<0,
		UPDATE_ORBIT = 1<<1,
		UPDATE_FOV = 1<<2,
		UPDATE_JOINT = 1<<3,
		UPDATE_OBJECT = 1<<4,
	};

	maPoint3d  m_Pos;
	float m_Orbit;
	float m_FOV;			//in degrees
	std::string m_Joint;
	std::string m_Object;
	int m_UpdateFlag;
};


//============================================================================
//============================================================================
class cmraCameraData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraCameraData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraCameraData(const cmraCameraData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~cmraCameraData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const cmraCameraData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraCameraData& operator=(const cmraCameraData& i_Data);

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyName		m_Name;
	prtyText		m_Description;
	prtyPoint3d		m_Position;
	// Note: there shouldn't be an orientation field here, 
	// cameras don't use it.
	prtyRotation	m_Orientation;

	prtyPoint3d		m_Target;
	prtyFloat		m_FOV;		// degrees wide camera angle
	prtyFloat		m_Tilt;		// degrees tilt
	prtyFloat		m_Near;		// near clipping plane
	prtyFloat		m_Far;		// far clipping plane

	//prtyBoolean		m_bMatchAspectToWindow;	// disables m_AspectRatio property below
	//prtyFloat		m_AspectRatio;

	prtyBoolean		m_bOrthographic;
	prtyFloat		m_OrthoWidth;

	prtyBoolean		m_bEnableDOF;
	prtyFloat		m_NearBlurDistance;
	prtyFloat		m_NearFocalDistance;
	prtyFloat		m_FarFocalDistance;
	prtyFloat		m_FarBlurDistance;
	// cap the blurriness at this value for far objects
	prtyFloat		m_MaxFarBlur;

	// HDR middle gray level
	prtyFloat		m_HDRMiddleGray;
	// HDR Bloom Scale
	prtyFloat		m_HDRBloomScale;
	// HDR Star Scale
	prtyFloat		m_HDRStarScale;
	// HDR Bright Pass Threshold
	prtyFloat		m_HDRBrightPassThresh;
	// HDR Bright Pass Offset
	prtyFloat		m_HDRBrightPassOffset;
	// HDR White Cutoff
	prtyFloat		m_HDRWhiteCutoff;
	// HDR Star Type
	prtyEnum		m_HDRStarType;
	// if adaptive luminance is unchecked, then use this value for luminance
	prtyFloat		m_HDRSceneLuminance;

};

