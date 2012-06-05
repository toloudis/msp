/****************************************************************************\
**	mexpRecorderDirectorsCut.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpRecorderDirectorsCut.hpp"

#include "Support/mexp/mexpExporter.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"

#include "Graphics/cam/camCamera.hpp"

namespace
{
} // end of anonymous namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderDirectorsCut::mexpRecorderDirectorsCut(const std::string& i_Name,
												   int i_DirectorsCutIndex)
:	m_Name(i_Name), 
	m_DirectorsCutIndex(i_DirectorsCutIndex)
{
}

//--------------------------------------------------------------------
//	Record frame of data at given time
//--------------------------------------------------------------------
void mexpRecorderDirectorsCut::RecordFrame(float i_CurrentTime)
{
	// Get camera being indexed currently and then record its values
	// into separate recorders
	int cam_index = camsDirectorsCutMgr::GetCameraIndex(this->m_DirectorsCutIndex);
	if (cam_index >=0 && cam_index < camsCameraMgr::GetNumCameras())
	{
		camCamera *pCamera = camsCameraMgr::GetCamera(cam_index);
		m_RecorderPosition.SubmitValue(i_CurrentTime, pCamera->GetPosition());
		m_RecorderTarget.SubmitValue(i_CurrentTime, pCamera->GetTarget());
		m_RecorderFOV.SubmitValue(i_CurrentTime, pCamera->GetFOV());
	}
}

//--------------------------------------------------------------------
//	End of recording, add delayed data to keys
//--------------------------------------------------------------------
void mexpRecorderDirectorsCut::FinishRecording()
{
	m_RecorderPosition.FinishRecording();
	m_RecorderTarget.FinishRecording();
	m_RecorderFOV.FinishRecording();

}

//--------------------------------------------------------------------
//	Call Export on all recorders
//--------------------------------------------------------------------
void mexpRecorderDirectorsCut::Export(mexpExporter &io_Exporter)
{
	io_Exporter.ExportCameraAnimationPosition(m_Name, m_RecorderPosition.m_Keys);
	io_Exporter.ExportCameraAnimationTarget(m_Name, m_RecorderTarget.m_Keys);
	io_Exporter.ExportCameraAnimationFOV(m_Name, m_RecorderFOV.m_Keys);
}

