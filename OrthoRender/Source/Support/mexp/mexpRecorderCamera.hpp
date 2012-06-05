/****************************************************************************\
**	mexpRecorderCamera.hpp
**
**		mexpRecorderCamera records changes in a camera's position or target
**	property into animation keys.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_RECORDERCAMERA_HPP
#error mexpRecorderCamera.hpp multiply included
#endif
#define MEXP_RECORDERCAMERA_HPP

#ifndef MEXP_CHANNELRECORDER_HPP
#include "Support/mexp/mexpChannelRecorder.hpp"
#endif

#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

//============================================================================
//============================================================================
class mexpRecorderCameraPoint3d : public mexpRecorderPropertyTemplate<maPoint3d, prtyPoint3d>
{
public:
	enum ChannelType
	{
		e_Position = 0,
		e_Target
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	explicit mexpRecorderCameraPoint3d(const std::string& i_Name,
								const prtyPoint3d &i_Property,
								ChannelType i_Type);

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter);

private:
	std::string m_Name;
	ChannelType m_ChannelType;
};

//============================================================================
//============================================================================
class mexpRecorderCameraFloat  : public mexpRecorderPropertyTemplate<float, prtyFloat>
{
public:
	enum ChannelType
	{
		e_FOV = 0
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	explicit mexpRecorderCameraFloat(const std::string& i_Name,
								const prtyFloat &i_Property,
								ChannelType i_Type);

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter);

private:
	std::string m_Name;
	ChannelType m_ChannelType;
};
