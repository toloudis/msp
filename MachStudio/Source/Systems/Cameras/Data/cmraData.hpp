/********************************************************************************************\
**  cmraData.hpp
**
**
**  StudioGPU
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
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif


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
	prtyBoolean				m_bEditorVisible;
	prtyName				m_Name;
	prtyText				m_Description;
	prtyPoint3dTransform	m_Position;
	// Note: there shouldn't be an orientation field here, 
	// cameras don't use it.
	prtyRotation			m_Orientation;

	prtyPoint3dTransform	m_Target;
	prtyFloat				m_FOV;		// degrees wide camera angle
	prtyFloat				m_Tilt;		// degrees tilt
	prtyDistance			m_Near;		// near clipping plane
	prtyDistance			m_Far;		// far clipping plane

	//prtyBoolean		m_bMatchAspectToWindow;	// disables m_AspectRatio property below
	//prtyFloat		m_AspectRatio;

	prtyBoolean				m_bOrthographic;
	prtyDistance			m_OrthoWidth;

	prtyBoolean				m_bEnableDOF;
	prtyDistance			m_NearBlurDistance;
	prtyDistance			m_NearFocalDistance;
	prtyDistance			m_FarFocalDistance;
	prtyDistance			m_FarBlurDistance;
	// cap the blurriness at this value for far objects
	prtyFloat				m_MaxFarBlur;
	prtyFloat				m_MaxCoC;

	//--------- Real world camera properties -----------
	prtyBoolean				m_bEnableALP;
	prtyFloat				m_FocalLength;
	prtyFloat				m_HorizontalAperture;
	prtyFloat				m_Fstop;
	prtyDistance			m_FocalDistance;
	prtyFloat				m_CoC;

	//--------End of real world camera properties---------


	// HDR middle gray level
	prtyFloat				m_HDRMiddleGray;
	// HDR Bloom Scale
	prtyFloat				m_HDRBloomScale;
	// HDR Star Scale
	prtyFloat				m_HDRStarScale;
	// HDR Bright Pass Threshold
	prtyFloat				m_HDRBrightPassThresh;
	// HDR Bright Pass Offset
	prtyFloat				m_HDRBrightPassOffset;
	// HDR White Cutoff
	prtyFloat				m_HDRWhiteCutoff;
	// HDR Star Type
	prtyEnum				m_HDRStarType;
	// if adaptive luminance is unchecked, then use this value for luminance
	prtyFloat				m_HDRSceneLuminance;

	prtyDistance			m_StereoFD;
	prtyEnum				m_StereoFilterColor;
	prtyEnum				m_StereoType;
	prtyEnum				m_StereoProjection;
	prtyDistance			m_StereoIOD;

	prtyTextureFileName		m_TextureFilenameAO;
	prtyFloat				m_IntensityAO;
	prtyInt32				m_BlendOpAO;

	prtyTextureFileName		m_TextureFilenameGI;
	prtyFloat				m_IntensityGI;
	prtyInt32				m_BlendOpGI;

	prtyTextureFileName		m_TextureFilenameRefl;
	prtyFloat				m_IntensityRefl;
	prtyInt32				m_BlendOpRefl;

	prtyTextureFileName		m_TextureFilenameShadowMask;
	prtyFloat				m_IntensityShadowMask;
	prtyInt32				m_BlendOpShadowMask;

	prtyTextureFileName		m_TextureFilenameBeauty;
	prtyFloat				m_IntensityBeauty;
	prtyInt32				m_BlendOpBeauty;
};

