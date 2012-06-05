/********************************************************************************************\
**  orthoRenderRequestTask.hpp
**
**		Data for the RenderRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_RENDERREQUESTTASK_HPP
#error orthoRenderRequestTask.hpp multiply included
#endif
#define ORTHO_RENDERREQUESTTASK_HPP

#ifndef LIGHTWAIT_RESPONSE_HPP
#include "MainApp/LightWaitResponse.hpp"
#endif
#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#include <string>


//============================================================================
//	render out a sequence (all capture drivers)
//============================================================================
class orthoRenderRequestTask : public orthoRequestTask
{
public:
	orthoRenderRequestTask( const std::string& i_TaskName, TaskIDs i_TaskID ) :
	  m_CameraName("FRONTAL"),
	  m_ImageFormat("JPG"),
	  m_MaskFormat("NONE"),
	  m_Width(256),
	  m_Height(256),
	  m_StartWidth(256),
	  m_StartHeight(256),
	  m_fTime(0.0f),
	  m_fFrameRate(24.0),
	  m_CurrentRenderResponse( NULL ),
	  orthoRequestTask( i_TaskName, i_TaskID){};

	void CreateRenderResponse( const std::string& modelName, const std::string& renderName )
	{
		if( m_CurrentRenderResponse ) delete m_CurrentRenderResponse;
		m_CurrentRenderResponse = new LightWaitResponse((char *)modelName.c_str(), (char *)renderName.c_str());
	}
	LightWaitResponse* GetRenderResponse(){ return m_CurrentRenderResponse; }

	virtual void ExecuteTask();

	virtual void SetCameraName( const std::string& i_CameraName ){ m_CameraName = i_CameraName; }
	virtual void SetImageFormat( const std::string& i_ImageFormat ){ m_ImageFormat = i_ImageFormat; }
	virtual void SetMaskFormat( const std::string& i_MaskFormat ){ m_MaskFormat = i_MaskFormat; }
	virtual void SetWidth( int i_Width ){ m_Width = i_Width; }
	virtual void SetHeight( int i_Height ){ m_Height = i_Height; }
	virtual void SetStartWidth( int i_StartWidth ){ m_StartWidth = i_StartWidth; }
	virtual void SetStartHeight( int i_StartHeight ){ m_StartHeight = i_StartHeight; }
	virtual void SetTime( float i_Time ){ m_fTime = i_Time; }
	virtual void SetFrameRate( float i_FrameRate ){ m_fFrameRate = i_FrameRate; }

	std::string	m_CameraName;
	std::string	m_ImageFormat;
	std::string	m_MaskFormat;
	int			m_Width;
	int			m_Height;
	int			m_StartWidth;
	int			m_StartHeight;
	float		m_fTime;
	float		m_fFrameRate;

private:
	LightWaitResponse *m_CurrentRenderResponse;

};

class orthoRenderRequestSequenceTask : public orthoRenderRequestTask
{
public:
	orthoRenderRequestSequenceTask() : orthoRenderRequestTask("sequence", SEQUENCE_ID ){};
	virtual void ExecuteTask();
};

//============================================================================
//	render out a frame of a camera
//============================================================================
class orthoRenderRequestFrameTask : public orthoRenderRequestTask
{
public:
	orthoRenderRequestFrameTask() : orthoRenderRequestTask("frame", FRAME_ID ){};
	virtual void ExecuteTask();
};
