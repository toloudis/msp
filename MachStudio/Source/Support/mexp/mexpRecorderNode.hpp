/****************************************************************************\
**	mexpRecorderNode.hpp
**
**		mexpRecorderNode records changes in a g3dSceneNode
**	into animation keys.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_RECORDERNODE_HPP
#error mexpRecorderNode.hpp multiply included
#endif
#define MEXP_RECORDERNODE_HPP

#ifndef MEXP_CHANNELRECORDER_HPP
#include "Support/mexp/mexpChannelRecorder.hpp"
#endif

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


class g3dSceneNode;

//============================================================================
//============================================================================
class mexpRecorderNodePosition : public mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderNodePosition(const std::string& i_NodeName,
					 const g3dSceneNode &i_Node);

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
	std::string m_NodeName;
	const g3dSceneNode &m_Node;
	mexpRecorderTemplate<maPoint3d> m_Recorder;
};

//============================================================================
//============================================================================
class mexpRecorderNodeRotation : public mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderNodeRotation(const std::string& i_NodeName,
					 const g3dSceneNode &i_Node);

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
	std::string m_NodeName;
	const g3dSceneNode &m_Node;
	mexpRecorderTemplate<maRotation> m_Recorder;
};
