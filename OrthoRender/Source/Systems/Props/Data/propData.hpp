/********************************************************************************************\
**  propData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PROP_DATA_HPP
#error propData.hpp multiply included
#endif
#define PROP_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
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
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif



//============================================================================
//============================================================================
class propData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propData(const itString& i_Filename,
			 const maPoint3d& i_Position,
			 const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const propData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propData& operator=(const propData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyBoolean		m_bVisible;
	prtyName		m_Name;
	prtyFileName	m_Filename;
	prtyPoint3d		m_Position;
	prtyRotation	m_Orientation;
	prtyFloat		m_Scale;
	prtyPoint3d		m_PivotPoint;
	prtyVector3d	m_PivotCompensation;
	prtyBoolean		m_bLockedMaterials;
};
