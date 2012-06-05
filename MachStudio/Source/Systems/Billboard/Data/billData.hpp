/********************************************************************************************\
**  billData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef BILL_DATA_HPP
#error billData.hpp multiply included
#endif
#define BILL_DATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
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
#ifndef PRTY_VIDEO_HPP
#include "Core/prty/prtyVideo.hpp"
#endif


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class billData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	billData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	billData(const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	billData(	const fsLocator& i_Filename,
				const maPoint3d& i_Position,
				float i_Scale );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const billData& i_Item);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyBoolean		m_bVisible;
	prtyName		m_Name;
	prtyFilePath	m_Filename;
	prtyPoint3d		m_Position;
	prtyRotation	m_Orientation;
	prtyDistance	m_Scale;
	prtyPoint3d		m_NonUniformScale;
	prtyBoolean		m_bOrientToCamera;
	prtyBoolean		m_bAdditiveMaterial;
	prtyColor		m_Color;
	prtyFloat		m_Brightness;
	prtyVideo		m_Video;

	prtyBoolean		m_bCKActive;
	prtyColor		m_CKColor;
	prtyFloat		m_CKTolerance;
	prtyBoolean		m_bCKRemoveSpill;
	prtyEnum		m_CKSpillType;
	prtyFloat		m_CKSpillBias;
	prtyInt32		m_CKEdgeBlur;

	prtyBoolean		m_bSnapToCamera;
	prtyFloat		m_DistToCamera;
	prtyEnum		m_CameraIndex;
};

