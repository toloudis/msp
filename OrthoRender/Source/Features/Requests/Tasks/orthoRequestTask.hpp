/********************************************************************************************\
**  orthoRequestTask.hpp
**
**		Request data
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_REQUESTTASK_HPP
#error orthoRequestTask.hpp multiply included
#endif
#define ORTHO_REQUESTTASK_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif
#ifndef CMRA_DATA_HPP
#include "Systems/Cameras/Data/cmraData.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//  Base class for all Requested tasks
//  New tasks must have a unique (Name) and (ID), the ID must be placed in the list for it's sort priority
// All Inherited class must override the ExecuteTask function, also to support dynamic property setting
// Set functions must be virtual and duplicated in the base class.  The are not pure since we only want to
// call the function on the implemented class and post an error if called in the base class
//============================================================================
class orthoRequestTask
{
public:
	//note task IDs determine execution order, lower first.
	enum TaskIDs
	{
		UNKNOWN_ID = 0,
		JOBINFO_ID,			//job info
		SCENE_ID,			//scene
		MODEL_ID,			//models
		MODELS_ID,
		DELETEALLANIMATION_ID,	//modifications
		ADDANIMATION_ID,
		DELETECHARACTER_ID,
		ATTACHMENT_ID,
		DETACHMENT_ID,
		SHOWCHARACTERS_ID,
		HIDECHARACTERS_ID,
		MATERIAL_ID,
		TEXTURE_ID,
		COLOR_ID,
		SCALECHARACTER_ID,
		ROTATECHARACTER_ID,
		EXPRESSION_ID,
		CHANGECAMERA_ID,
		SEQUENCE_ID,		//renders
		FRAME_ID,
		SAVEXMLSCENE_ID,	//CMS
		SAVESCENE_ID,
		SAVEXMLCHARACTER_ID,
	};

	orthoRequestTask(std::string i_TaskName, TaskIDs i_TaskID )
		: m_TaskName( i_TaskName ), m_TaskID( i_TaskID )
	{};

	virtual ~orthoRequestTask() {};

	virtual void ExecuteTask() = 0;

	//ascending sorting operator
	static bool SortLess( const orthoRequestTask* first, const orthoRequestTask* second );

	const std::string& GetTaskName(){ return m_TaskName; }
	TaskIDs            GetTaskID(){ return m_TaskID; }

private:

	TaskIDs m_TaskID;
	std::string	m_TaskName;

public:
	//If any of the following get called then the function was not properly overridden from the inherited class.
	virtual void SetJobName( const std::string& ){ DBG_ERROR( "JobName value has invalid placement." ); }
	virtual void SetIsBaseModel( bool ){ DBG_ERROR( "BaseModel value has invalid placement." ); }
	virtual void SetCameraName( const std::string& ){ DBG_ERROR( "CameraName value has invalid placement." ); }
	virtual void SetImageFormat( const std::string& ){ DBG_ERROR( "ImageFormat value has invalid placement." ); }
	virtual void SetMaskFormat( const std::string& ){ DBG_ERROR( "MaskFormat value has invalid placement." ); }
	virtual void SetWidth( int ){ DBG_ERROR( "Width value has invalid placement." ); }
	virtual void SetHeight( int ){ DBG_ERROR( "Height value has invalid placement." ); }
	virtual void SetStartWidth( int ){ DBG_ERROR( "StartWidth value has invalid placement." ); }
	virtual void SetStartHeight( int ){ DBG_ERROR( "StartHeight value has invalid placement." ); }
	virtual void SetTime( float ){ DBG_ERROR( "Time value has invalid placement." ); }
	virtual void SetFrameRate( float ){ DBG_ERROR( "FrameRate value has invalid placement." ); }
	virtual void SetName( const std::string& ){ DBG_ERROR( "Name value has invalid placement." ); }
	virtual void SetFileName( const std::string& ){ DBG_ERROR( "FileName value has invalid placement." ); }
	virtual void SetAngle( float ){ DBG_ERROR( "Angle value has invalid placement." );}
	virtual void SetJointName( const std::string& ){ DBG_ERROR( "JointName value has invalid placement." ); }
	virtual void SetLayerName( const std::string& ){ DBG_ERROR( "LayerName value has invalid placement." ); }
	virtual void SetShaderName( const std::string& ){ DBG_ERROR( "ShaderName value has invalid placement." ); }
	virtual void SetAnimationName( const std::string& ){ DBG_ERROR( "AnimationName value has invalid placement." ); }
	virtual void SetDirections( int ){ DBG_ERROR( "Directions value has invalid placement." ); }
	virtual void SetColor( const maFloatRGBA& ){ DBG_ERROR( "Color value has invalid placement." ); }
	virtual maVector3d& GetVector(){ static maVector3d ma; DBG_ERROR( "GetVector value has invalid placement." ); return ma; }
	//camera
	virtual void SetCameraOrbit( float ){ DBG_ERROR( "CameraOrbit value has invalid placement." ); }
	virtual void SetCameraFOV( float ){ DBG_ERROR( "CameraFOV value has invalid placement." ); }
	virtual void SetObjectName( const std::string& ){ DBG_ERROR( "ObjectName value has invalid placement." ); }
	virtual void SetWeight( const float i_Weight ){ DBG_ERROR( "Weight value has invalid placement." ); }
};

typedef std::vector<orthoRequestTask*> orthoRequestJob;	//a job with a list of tasks
typedef std::vector<orthoRequestJob*> orthoRequestJobs;	//list of jobs
