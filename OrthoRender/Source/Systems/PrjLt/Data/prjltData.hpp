/********************************************************************************************\
**  prjltData.hpp
**
**
**  Extra Large Technology
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
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
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
	prtyColor		m_Color;
	prtyBoolean		m_Enabled;
	prtyBoolean		m_ShadowSource;
	prtyPoint3d		m_Falloff;
	prtyFloat		m_Range;
	prtyFloat		m_Intensity;

	// Note: Direction is now changed to Target
	//prtyVector3d  m_Direction;
	prtyPoint3d		m_Target;
	prtyFloat		m_Angle;
	prtyFloat		m_Scale;
	prtyFloat		m_Aspect;
	prtyFloat		m_Tilt;

	prtyFileName	m_TextureFilename;
	prtyInt32		m_DepthMapSize;

	// params for soft shadow algorithm
	prtyFloat		m_LightSize;
	prtyFloat		m_SceneScale;
	prtyBoolean		m_ShadowQuality;
	
	prtyFloat		m_ShadowIntensity;
	prtyFloat		m_DepthBias;
	prtyColor		m_ShadowColor;

	prtyBoolean		m_bEditorVisible;
	prtyName		m_Name;
	prtyPoint3d		m_Position;
	// Projected lights shouldn't have an orientation field
	prtyRotation	m_Orientation;

	prtyBoolean		m_bDiffuseEnabled;
	prtyBoolean		m_bSpecularEnabled;

	prtyBoolean		m_bShaftVisible;
	prtyFloat		m_ShaftAlpha;
	prtyFloat		m_ShaftDensity;
	prtyFloat		m_ShaftDistFalloffStart;
	prtyFloat		m_ShaftDistFalloffEnd;
	prtyFileName	m_ShaftTextureFilename;

	prtyBoolean		m_bAffectsFur;
	prtyBoolean		m_bAffectsGlow;

};

