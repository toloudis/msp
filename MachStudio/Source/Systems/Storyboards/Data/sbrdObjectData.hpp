/********************************************************************************************\
**  sbrdObjectData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef SBRD_OBJECTDATA_HPP
#error sbrdObjectData.hpp multiply included
#endif
#define SBRD_OBJECTDATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

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
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class sbrdObjectData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdObjectData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdObjectData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdObjectData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					float i_Scale );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const sbrdObjectData& i_Item);

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
	prtyBoolean		m_bOrientToCamera;
	prtyBoolean		m_bAdditiveMaterial;
	prtyColor		m_Color;
};

