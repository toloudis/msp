/****************************************************************************\
**	mexpRecorderRotation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpRecorderRotation.hpp"

#include "Support/mexp/mexpExporter.hpp"


namespace
{
} // end of anonymous namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderRotation::mexpRecorderRotation(const std::string& i_AssetName,
										   const prtyRotation &i_Property)
: m_Property(i_Property),
	m_AssetName(i_AssetName)
{

}

//--------------------------------------------------------------------
//	Export data to the exporter
//--------------------------------------------------------------------
void mexpRecorderRotation::Export(mexpExporter &io_Exporter)
{
	io_Exporter.ExportAnimationRotation(m_AssetName, m_Recorder.m_Keys);
}


//--------------------------------------------------------------------
//	Record frame of data at given time
//--------------------------------------------------------------------
//virtual 
void mexpRecorderRotation::RecordFrame(float i_CurrentTime)
{
	m_Recorder.SubmitValue(i_CurrentTime, m_Property.GetQuaternion());
}

//--------------------------------------------------------------------
//	End of recording, add delayed data to keys
//--------------------------------------------------------------------
//virtual 
void mexpRecorderRotation::FinishRecording()
{
	m_Recorder.FinishRecording();
}