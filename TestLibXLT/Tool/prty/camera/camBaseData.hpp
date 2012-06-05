//
//		A base data class for an cam system object
//
#ifdef CAM_BASEDATA_HPP
#error camBaseData.hpp multiply included
#endif
#define CAM_BASEDATA_HPP

#ifndef PRTY_ANGLE_HPP
#include "Core/prty/prtyAngle.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_HOTKEY_HPP
#include "Core/prty/prtyHotKey.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_RECT_HPP
#include "Core/prty/prtyRect.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif


//============================================================================
//============================================================================
class camBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	camBaseData()
	:	m_Position("Position"),
		m_Orientation("Orientation", maVector3d(0.0f,0.0f,0.0f)),
		m_Description("Description"),
		m_Angle("Mystery Angle"),
		m_LightColor("Color"),
		m_Viewport("Viewport Size"),
		m_Twist("Pitch"),
		m_CamName("Camera Name"),
		m_HotKey("Hot Key")
	{
		m_Position.SetValue(2.0f, 2.0f, 3.0f);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	camBaseData(camBaseData& i_Data)
	:	m_Position("Position", i_Data.m_Position.GetValue()),
		m_Orientation("Orientation", i_Data.m_Orientation.GetValue()),
		m_Description("Description", i_Data.m_Description.GetValue()),
		m_Angle("Mystery Angle", i_Data.m_Angle.GetValue()),
		m_LightColor("Color", i_Data.m_LightColor.GetValue()),
		m_Viewport("Viewport Size", i_Data.m_Viewport.GetValue()),
		m_Twist("Pitch", i_Data.m_Twist.GetQuaternion()),
		m_CamName("Camera Name"),
		m_HotKey("Hot Key",i_Data.m_HotKey.GetValue())
	{
		m_Position.SetValue(2.0f, 2.0f, 1.0f);
	}

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyPoint3d		m_Position;
	prtyVector3d	m_Orientation;
	prtyText		m_Description;
	prtyAngle		m_Angle;
	prtyColor		m_LightColor;
	prtyRect		m_Viewport;
	prtyRotation	m_Twist;
	prtyName		m_CamName;
	prtyHotKey		m_HotKey;
};

