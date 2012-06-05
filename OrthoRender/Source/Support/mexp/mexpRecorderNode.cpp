/****************************************************************************\
**	mexpRecorderNode.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpRecorderNode.hpp"

#include "Support/mexp/mexpExporter.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"

namespace
{
} // end of anonymous namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderNodePosition::mexpRecorderNodePosition(const std::string& i_NodeName,
												   const g3dSceneNode &i_Node)
: m_NodeName(i_NodeName), m_Node(i_Node)
{

}

//--------------------------------------------------------------------
//	Record frame of data at given time
//--------------------------------------------------------------------
void mexpRecorderNodePosition::RecordFrame(float i_CurrentTime)
{
	// Convert from matrix to translation
	const maMatrix4x4& matx = m_Node.GetTransform();
	maPoint3d position = matx.GetTranslation();

	m_Recorder.SubmitValue(i_CurrentTime, position);
}

//--------------------------------------------------------------------
//	End of recording, add delayed data to keys
//--------------------------------------------------------------------
void mexpRecorderNodePosition::FinishRecording()
{
	m_Recorder.FinishRecording();
}

//--------------------------------------------------------------------
//	Export data to the exporter
//--------------------------------------------------------------------
void mexpRecorderNodePosition::Export(mexpExporter &io_Exporter)
{
	io_Exporter.ExportAnimationTranslation(m_NodeName, m_Recorder.m_Keys);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderNodeRotation::mexpRecorderNodeRotation(const std::string& i_NodeName,
												   const g3dSceneNode &i_Node)
: m_NodeName(i_NodeName), m_Node(i_Node)
{

}

//--------------------------------------------------------------------
//	Record frame of data at given time
//--------------------------------------------------------------------
void mexpRecorderNodeRotation::RecordFrame(float i_CurrentTime)
{
	// Convert from matrix to rotation
	const maMatrix4x4& matx = m_Node.GetTransform();
	maRotation orientation;
	orientation.SetValue(matx);

	m_Recorder.SubmitValue(i_CurrentTime, orientation);
}

//--------------------------------------------------------------------
//	End of recording, add delayed data to keys
//--------------------------------------------------------------------
void mexpRecorderNodeRotation::FinishRecording()
{
	m_Recorder.FinishRecording();
}

//--------------------------------------------------------------------
//	Export data to the exporter
//--------------------------------------------------------------------
void mexpRecorderNodeRotation::Export(mexpExporter &io_Exporter)
{
	io_Exporter.ExportAnimationRotation(m_NodeName, m_Recorder.m_Keys);
}

