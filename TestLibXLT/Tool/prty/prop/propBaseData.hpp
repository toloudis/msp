//
//		A base data class for an prop system object
//
#ifdef PROP_BASEDATA_HPP
#error propBaseData.hpp multiply included
#endif
#define PROP_BASEDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_MATRIX4X4_HPP
#include "Core/prty/prtyMatrix4x4.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif


//============================================================================
//============================================================================
class propBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propBaseData()
	:	m_bEditorVisible("Visible in Editor",true),
		m_FOV("Field of View",34.4f),
		m_Tilt("Tilt"),
		m_Near("Near Clip",23.4f),
		m_Far("Far Clip",1000.99f),
		m_Position("Position"),
		m_ID("ID"),
		m_Count("Count"),
		m_WorldMatrix("World Matrix"),
		m_Description("Description")
	{
		m_Position.SetValue(7.0f, 7.0f, 7.0f);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propBaseData(propBaseData& i_Data)
	:	m_bEditorVisible("Visible in Editor",i_Data.m_bEditorVisible.GetValue()),
		m_FOV("Field of View",i_Data.m_FOV.GetValue()),
		m_Tilt("Tilt",i_Data.m_Tilt.GetValue()),
		m_Near("Near Clip",i_Data.m_Near.GetValue()),
		m_Far("Far Clip",i_Data.m_Far.GetValue()),
		m_Position("Position", i_Data.m_Position.GetValue()),
		m_ID("ID", i_Data.m_ID.GetValue()),
		m_Count("Count", i_Data.m_Count.GetValue()),
		m_WorldMatrix("World Matrix", i_Data.m_WorldMatrix.GetValue()),
		m_Description("Description")
	{
		m_Position.SetValue(7.0f, 7.0f, 6.0f);
	}

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyFloat		m_FOV;
	prtyFloat		m_Tilt;
	prtyFloat		m_Near;
	prtyFloat		m_Far;
	prtyVector3d	m_Position;
	prtyInt8		m_ID;
	prtyInt32		m_Count;
	prtyMatrix4x4	m_WorldMatrix;
	prtyText		m_Description;
};

