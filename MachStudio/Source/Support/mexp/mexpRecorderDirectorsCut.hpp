/****************************************************************************\
**	mexpRecorderDirectorsCut.hpp
**
**		mexpRecorderDirectorsCut follows the camera cuts from a directors
**	cut and exports it as a single camera into Maya.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_RECORDERDIRECTORSCUT_HPP
#error mexpRecorderDirectorsCut.hpp multiply included
#endif
#define MEXP_RECORDERDIRECTORSCUT_HPP

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
class mexpRecorderDirectorsCut : public mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderDirectorsCut(const std::string& i_Name,
							 int i_DirectorsCutIndex);

	//--------------------------------------------------------------------
	//	Record frame of data at given time
	//--------------------------------------------------------------------
	virtual void RecordFrame(float i_CurrentTime);

	//--------------------------------------------------------------------
	//	End of recording, add delayed data to keys
	//--------------------------------------------------------------------
	virtual void FinishRecording();

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter);

private:
	std::string m_Name;
	int m_DirectorsCutIndex;

	// Contains recorders for each channel it will animate
	mexpRecorderTemplate<maPoint3d> m_RecorderPosition;
	mexpRecorderTemplate<maPoint3d> m_RecorderTarget;
	mexpRecorderTemplate<float> m_RecorderFOV;
};
