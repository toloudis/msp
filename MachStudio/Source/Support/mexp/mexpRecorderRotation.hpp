/****************************************************************************\
**	mexpRecorderRotation.hpp
**
**		mexpRecorderRotation records changes in a rotation property
**	into animation keys.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_RECORDERROTATION_HPP
#error mexpRecorderRotation.hpp multiply included
#endif
#define MEXP_RECORDERROTATION_HPP

#ifndef MEXP_CHANNELRECORDER_HPP
#include "Support/mexp/mexpChannelRecorder.hpp"
#endif

#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//============================================================================
//============================================================================
class mexpRecorderRotation : public mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderRotation(const std::string& i_AssetName,
		const prtyRotation &i_Property);

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter);

	//--------------------------------------------------------------------
	//	Record frame of data at given time
	//--------------------------------------------------------------------
	virtual void RecordFrame(float i_CurrentTime);

	//--------------------------------------------------------------------
	//	End of recording, add delayed data to keys
	//--------------------------------------------------------------------
	virtual void FinishRecording();

private:
	std::string m_AssetName;
	const prtyRotation &m_Property;
	mexpRecorderTemplate<maRotation> m_Recorder;
};
