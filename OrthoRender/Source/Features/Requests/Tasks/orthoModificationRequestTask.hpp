/********************************************************************************************\
**  orthoModificationRequestTask.hpp
**
**		Data for the ModificationRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_MODIFICATIONREQUESTTASK_HPP
#error orthoModificationRequestTask.hpp multiply included
#endif
#define ORTHO_MODIFICATIONREQUESTTASK_HPP

#ifndef LIGHTWAIT_RESPONSE_HPP
#include "MainApp/LightWaitResponse.hpp"
#endif
#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif
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


//============================================================================
//============================================================================
class orthoModificationRequestTask : public orthoRequestTask
{
public:
	orthoModificationRequestTask( std::string i_TaskName, TaskIDs i_TaskID ) : orthoRequestTask( i_TaskName, i_TaskID ){};
	virtual void ExecuteTask(){}

	//If any of the following get called then the function was not properly overridden from the inherited class.
	virtual void SetName( const std::string& ){ DBG_ERROR( "Name value has invalid placement." ); }
	virtual void SetFileName( const std::string& ){ DBG_ERROR( "FileName value has invalid placement." ); }
	virtual void SetAngle( float ){ DBG_ERROR( "Angle value has invalid placement." );}
	virtual void SetJointName( const std::string& ){ DBG_ERROR( "JointName value has invalid placement." ); }
	virtual void SetLayerName( const std::string& ){ DBG_ERROR( "LayerName value has invalid placement." ); }
	virtual void SetShaderName( const std::string& ){ DBG_ERROR( "ShaderName value has invalid placement." ); }
	virtual void SetAnimationName( const std::string& ){ DBG_ERROR( "AnimationName value has invalid placement." ); }
	virtual void SetCameraName( const std::string& ){ DBG_ERROR( "CameraName value has invalid placement." ); }
	virtual void SetDirections( int ){ DBG_ERROR( "Directions value has invalid placement." ); }
	virtual void SetColor( const maFloatRGBA& ){ DBG_ERROR( "Color value has invalid placement." ); }
	virtual maVector3d& GetVector(){ static maVector3d ma; DBG_ERROR( "GetVector value has invalid placement." ); return ma; }
	//camera
	virtual void SetCameraOrbit( float ){ DBG_ERROR( "CameraOrbit value has invalid placement." ); }
	virtual void SetCameraFOV( float ){ DBG_ERROR( "CameraFOV value has invalid placement." ); }
	virtual void SetObjectName( const std::string& ){ DBG_ERROR( "ObjectName value has invalid placement." ); }

};

//============================================================================
//	Expressions
//============================================================================
class orthoModificationRequestExpressionTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestExpressionTask() : orthoModificationRequestTask("expression", EXPRESSION_ID ) {};

	virtual void ExecuteTask();

	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual void SetWeight( const float i_Weight ){ m_Weight = i_Weight; }

public:
	std::string	m_Name;
	std::string	m_FileName;
	float m_Weight;
};

//============================================================================
//	ShowCharacters
//============================================================================
class orthoModificationRequestShowCharacterTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestShowCharacterTask() : orthoModificationRequestTask("character", SHOWCHARACTERS_ID ) {};

	virtual void ExecuteTask();

	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_Name;
	std::string	m_FileName;
};

//============================================================================
//	HideCharacters
//============================================================================
class orthoModificationRequestHideCharacterTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestHideCharacterTask() : orthoModificationRequestTask("character", HIDECHARACTERS_ID ) {};

	virtual void ExecuteTask();

	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_Name;
	std::string	m_FileName;
};

//============================================================================
//	DeleteCharacters
//============================================================================
class orthoModificationRequestDeleteCharacterTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestDeleteCharacterTask() : orthoModificationRequestTask("character", DELETECHARACTER_ID ) {};

	virtual void ExecuteTask();

	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_FileName;
};


//============================================================================
//	Attachment
//============================================================================
class orthoModificationRequestAttachmentTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestAttachmentTask() : orthoModificationRequestTask("attachment", ATTACHMENT_ID ) {};

	virtual void ExecuteTask();

	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual void SetJointName( const std::string& i_JointName ){ m_JointName = i_JointName; }

public:
	std::string	m_Name;			// object name or filename
	std::string	m_JointName;
	std::string	m_FileName;
};

//============================================================================
//	Detachment
//============================================================================
class orthoModificationRequestDetachmentTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestDetachmentTask() : orthoModificationRequestTask("detachment", DETACHMENT_ID ) {};

	virtual void ExecuteTask();

	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual void SetJointName( const std::string& i_JointName ){ m_JointName = i_JointName; }

public:
	std::string	m_Name;			// object name or filename
	std::string	m_JointName;
	std::string	m_FileName;
};


//============================================================================
//	Texture
//============================================================================
class orthoModificationRequestTextureTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestTextureTask() : orthoModificationRequestTask("texture", TEXTURE_ID ) {};

	virtual void ExecuteTask();

	virtual void SetLayerName( const std::string& i_LayerName ){ m_LayerName = i_LayerName; }
	virtual void SetShaderName( const std::string& i_ShaderName ){ m_ShaderName = i_ShaderName; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_LayerName;
	std::string	m_ShaderName;
	std::string	m_FileName;
};


//============================================================================
//	Material
//============================================================================
class orthoModificationRequestMaterialTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestMaterialTask() : orthoModificationRequestTask("material", MATERIAL_ID ) {};

	virtual void ExecuteTask();

	virtual void SetShaderName( const std::string& i_ShaderName ){ m_SurfaceName = i_ShaderName; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_SurfaceName;
	std::string	m_FileName;
};


//============================================================================
//	Color
//============================================================================
class orthoModificationRequestColorTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestColorTask() : orthoModificationRequestTask("color", COLOR_ID ) {};

	virtual void ExecuteTask();

	virtual void SetSurfaceName( const std::string& i_SurfaceName ){ m_SurfaceName = i_SurfaceName; }
	virtual void SetColor( const maFloatRGBA& i_Color ){ m_Color = i_Color; }

public:
	std::string	m_SurfaceName;
	maFloatRGBA	m_Color;
};

//============================================================================
//	AnimationAdd
//============================================================================
class orthoModificationRequestAnimationAddTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestAnimationAddTask() : orthoModificationRequestTask("animation", ADDANIMATION_ID ) {};

	virtual void ExecuteTask();

	virtual void SetAnimationName( const std::string& i_AnimationName ){ m_AnimationName = i_AnimationName; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual void SetCameraName( const std::string& i_CameraName ){ m_CameraName = i_CameraName; }
	virtual void SetDirections( int i_Dir ){ m_Directions = i_Dir; }

public:
	std::string	m_AnimationName;
	std::string	m_FileName;
	std::string	m_CameraName;
	int			m_Directions;
};

//============================================================================
//	DeleteAllAnimation
//============================================================================
class orthoModificationRequestAnimationDeleteAllTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestAnimationDeleteAllTask() : orthoModificationRequestTask("deleteallanimation", DELETEALLANIMATION_ID ) {};

	virtual void ExecuteTask();
};

//============================================================================
//	ScaleCharacter
//============================================================================
class orthoModificationRequestScaleCharacterTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestScaleCharacterTask() : orthoModificationRequestTask("scale", SCALECHARACTER_ID ) {};

	virtual void ExecuteTask();

	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual maVector3d& GetVector(){ return m_Scale; }

public:
	std::string	m_FileName;
	maVector3d  m_Scale;
};

//============================================================================
//	RotateCharacter
//============================================================================
class orthoModificationRequestRotateCharacterTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestRotateCharacterTask() : orthoModificationRequestTask("rotate", ROTATECHARACTER_ID ) {};

	virtual void ExecuteTask();

	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }
	virtual void SetAngle( float i_Angle ){ m_Angle = i_Angle; }

public:
	std::string	m_FileName;
	float m_Angle;
};

//============================================================================
//	Camera Change
//============================================================================
class orthoModificationRequestChangeCameraTask : public orthoModificationRequestTask
{
public:
	orthoModificationRequestChangeCameraTask() : orthoModificationRequestTask("camera", CHANGECAMERA_ID ){};

	virtual void ExecuteTask();

	virtual void SetCameraName( const std::string& i_CameraName ){ m_CameraName = i_CameraName; }
	virtual maVector3d& GetVector(){ m_Data.m_UpdateFlag |= cmraCameraChangeData::UPDATE_POS; return m_Data.m_Pos; }
	virtual void SetCameraOrbit( float i_Orbit ){ m_Data.m_Orbit = i_Orbit; m_Data.m_UpdateFlag |= cmraCameraChangeData::UPDATE_ORBIT; }
	virtual void SetCameraFOV( float i_FOV ){ m_Data.m_FOV = i_FOV; m_Data.m_UpdateFlag |= cmraCameraChangeData::UPDATE_FOV; }
	virtual void SetJointName( const std::string& i_JointName ){ m_Data.m_Joint = i_JointName; m_Data.m_UpdateFlag |= cmraCameraChangeData::UPDATE_JOINT; }
	virtual void SetObjectName( const std::string& i_ObjectName ){ m_Data.m_Object = i_ObjectName; m_Data.m_UpdateFlag |= cmraCameraChangeData::UPDATE_OBJECT; }

public:
	std::string	m_CameraName;
	cmraCameraChangeData m_Data;
};



