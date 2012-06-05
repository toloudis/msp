/****************************************************************************\
**	cmraAvatarDataInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraAvatarDataInterest.hpp"

#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCapture.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

#include "Graphics/Cam/camCamera.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraAvatarDataInterest::cmraAvatarDataInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
cmraAvatarDataInterest::~cmraAvatarDataInterest()
{
}

//--------------------------------------------------------------------
// Only add the capture driver for the camera
//--------------------------------------------------------------------
void cmraAvatarDataInterest::AddAnimation( const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras )
{
	static const char s_DriverName[] = "Capture";
	
	bool bAll = false;
	int id = 0;
	int count = 0;

	if( i_Cameras.size() >= 1 && i_Cameras[0] == std::string("ALL"))
	{
		bAll = true;
		count = cmraObjectMgr::GetNumObjects();
	}
	else
	{
		count = i_Cameras.size();
	}

	for( int c = 0; c < count; c++ )
	{
		if( bAll )
		{
			id = c;
		}
		else
		{
			id = cmraObjectMgr::GetIndexForObject( i_Cameras[c] );
		}

		if( id >= 0 )
		{
			cmraScriptObject* pCam = cmraObjectMgr::GetObject( id );
			if( pCam )
			{
				DBG_LOG2("Adding capture to cam %d to cover anim (%s)", c, i_AnimFile.c_str());

				// get the drivers and find if the driver exists
				//
				tmlnDriverNameList driver_names;
				tmlnCreator::GatherPossibleDrivers( pCam, driver_names );

				if( !driver_names.Empty())
				{
					//	find the index
					//
					int i;
					for (i=0; i< driver_names.m_DriverNames.size(); i++)
					{
						if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),s_DriverName) == 0)
						{
							break;
						}
					}

					if ( i >= driver_names.m_DriverNames.size() )
					{
						return;
					}

					tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

					// Create Driver
					//
					tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( s_DriverName, pCam );

					// Give driver to script object to own
					if( pDriver )
					{
						pDriver->SetBeginTime( OrthoAvatarTemporalSpace::animStartTime );
						pDriver->SetEndTime( OrthoAvatarTemporalSpace::animEndTime );

						pCam->AddDriver( pDriver );
						pCam->NotifyDriverChanged();
					}
				}
			}
		}
	}
}


//--------------------------------------------------------------------
//	Delete All Drivers
//--------------------------------------------------------------------
void cmraAvatarDataInterest::DeleteSceneDrivers()
{
	for( int i = 0; i < cmraObjectMgr::GetNumObjects(); i++ )
	{
		cmraScriptObject* pCam = cmraObjectMgr::GetObject( i );
		if( pCam )
		{
//			int dcount = 0;
			int count = pCam->GetNumDrivers();
			while( count-- > 0 )
			{
				tmlnDriver* drv = pCam->GetDriverDirect(count);
				cmraDriverCapture* capdrv = dynamic_cast<cmraDriverCapture*>(drv);
				if( capdrv )
				{
					pCam->RemoveDriver( capdrv );
					delete capdrv;
//					dcount++;
				}
			}
//			if( dcount )
//			{
//				DBG_LOG1("Cleared %d capture driver(s) for cam %s", dcount, pCam->GetTmlnName().c_str() );
//			}
		}
	}
}

//--------------------------------------------------------------------
//	Change Camera
//--------------------------------------------------------------------
void cmraAvatarDataInterest::CameraChange( const std::string& i_Camera, const cmraCameraChangeData& i_Data )
{
	bool bAll = false;

	int id = cmraObjectMgr::GetIndexForObject( i_Camera );
	if( id >= 0 )
	{
		cmraScriptObject* pCam = cmraObjectMgr::GetObject( id );
		if( pCam )
		{
			maPoint3d cPos = pCam->Camera().GetPosition();
			maPoint3d cTar = pCam->Camera().GetTarget();
			maPoint3d cUp  = pCam->Camera().GetUp();

			if( i_Data.m_UpdateFlag & cmraCameraChangeData::UPDATE_POS )
			{
				maPoint3d cVec = cTar - cPos;
				pCam->Camera().LookAt( i_Data.m_Pos, i_Data.m_Pos + cVec, cUp);
			}
			if( i_Data.m_UpdateFlag & cmraCameraChangeData::UPDATE_ORBIT )
			{
				maRotation roty(maVector3d(0,1,0), i_Data.m_Orbit);

				maVector3d oVec = cPos - cTar;
				roty.RotateVector( oVec );

				pCam->Camera().LookAt( cTar + oVec, cTar, cUp);
			}
			if( i_Data.m_UpdateFlag & cmraCameraChangeData::UPDATE_FOV )
			{
				pCam->Camera().SetFOV( i_Data.m_FOV );
			}
			if( i_Data.m_UpdateFlag & cmraCameraChangeData::UPDATE_JOINT )
			{
				bool bFound = false;
				for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)	//search all models for the joint
				{
					//	this should be the character
					chtrScriptObject* pObj = chtrObjectMgr::GetObject(i);
					if( pObj != NULL )
					{
						api3dReference* pAPI = pObj->GetReference( i_Data.m_Joint );	//is the joint here?
						if( pAPI )
						{
							maVector3d tar(0,0,0);
							pAPI->GetMatrix().Transform( tar );			//Get the Joint position in world space.
							pCam->Camera().LookAt( cPos, tar, cUp);		//set as target position.
							bFound = true;
							break;
						}
					}
				}
				if( !bFound )
				{
					DBG_WARNING1( "CameraChange - Joint %s not found.", i_Data.m_Joint.c_str() );
				}
			}
			if( i_Data.m_UpdateFlag & cmraCameraChangeData::UPDATE_OBJECT )
			{
				bool bFound = false;
				for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)	//search all models
				{
					//	this should be the character
					chtrScriptObject* pObj = chtrObjectMgr::GetObject(i);
					if( pObj != NULL )
					{
						itString fname;
						pObj->GetFilename(fname);

						//	verify the name or filename is the same
						if( (pObj->GetName() == nameString(i_Data.m_Object))
							|| (fname == itString(i_Data.m_Object.c_str())) )
						{
//							maPoint3d pnt = pObj->GetBaseData().m_Position.GetValue();
							maPoint3d pnt2 = pObj->ChannelPosition().GetPosition();
//							maPoint3d pnt3 = pObj->GetPickObject()->GetPosition();
							pCam->Camera().LookAt( cPos, pnt2, cUp);		//set as target position.
							bFound = true;
							break;
						}
					}
				}
				if( !bFound )
				{
					DBG_WARNING1( "CameraChange - Object %s not found.", i_Data.m_Object.c_str() );
				}
			}
		}
	}
	else
	{
		DBG_LOG1("CameraChange: Invalid camera %s", i_Camera.c_str() );
	}
}

