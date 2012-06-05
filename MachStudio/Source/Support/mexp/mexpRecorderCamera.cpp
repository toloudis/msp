/****************************************************************************\
**	mexpRecorderCamera.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpRecorderCamera.hpp"

#include "Support/mexp/mexpExporter.hpp"

namespace
{
} // end of anonymous namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderCameraPoint3d::mexpRecorderCameraPoint3d(const std::string& i_Name,
								const prtyPoint3d &i_Property,
								ChannelType i_Type)
: mexpRecorderPropertyTemplate<maPoint3d, prtyPoint3d>(i_Property),
	m_Name(i_Name), 
	m_ChannelType(i_Type)
{

}

//--------------------------------------------------------------------
//	Call Export on all recorders
//--------------------------------------------------------------------
void mexpRecorderCameraPoint3d::Export(mexpExporter &io_Exporter)
{
	switch (m_ChannelType)
	{
	case e_Position:
		io_Exporter.ExportCameraAnimationPosition(m_Name, m_Recorder.m_Keys);
		break;
	case e_Target:
		io_Exporter.ExportCameraAnimationTarget(m_Name, m_Recorder.m_Keys);
		break;
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderCameraFloat::mexpRecorderCameraFloat(const std::string& i_Name,
								const prtyFloat &i_Property,
								ChannelType i_Type)
: mexpRecorderPropertyTemplate<float, prtyFloat>(i_Property),
	m_Name(i_Name), 
	m_ChannelType(i_Type)
{

}

//--------------------------------------------------------------------
//	Export data to the exporter
//--------------------------------------------------------------------
void mexpRecorderCameraFloat::Export(mexpExporter &io_Exporter)
{
	switch (m_ChannelType)
	{
	case e_FOV:
		io_Exporter.ExportCameraAnimationFOV(m_Name, m_Recorder.m_Keys);
		break;
	}
}

