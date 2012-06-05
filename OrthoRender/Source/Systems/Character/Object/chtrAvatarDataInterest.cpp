/****************************************************************************\
**	chtrAvatarDataInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrAvatarDataInterest.hpp"

#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelOrientation.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Support/fsys/fsysFileList.hpp"
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/Object/chtrObject.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"  // HACK - ugh!

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/Eff/effHairData.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/Eff/effSkinData.hpp"
#include "Graphics/Ent/entEntity.hpp"
#include "Graphics/Ent/entImport.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/Sc/scObject.hpp"


//============================================================================
//============================================================================
namespace
{
	tmlnAdapterReference l_Reference;

	void SetMaterialTexture(shared_ptr<effShaderParams> i_Shader, std::string i_ParamName, std::string i_TextureName)
	{
		effParamTexture* pTex = i_Shader->FindTextureParam(i_ParamName);
		if (pTex)
		{
			pTex->Property().SetValue(itString(i_TextureName.c_str()));
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrAvatarDataInterest::chtrAvatarDataInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
chtrAvatarDataInterest::~chtrAvatarDataInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrAvatarDataInterest::LoadCharacter( const std::string& i_Name, const std::string& i_FileName, const bool i_bIsBaseModel )
{
	////safe delete of the list of objects
	//while( chtrObjectMgr::GetNumObjects() > 0 )
	//{
	//	chtrObjectMgr::DeleteObject(0);
	//}

	//DBG_LOG1("Loading character (%s)", i_FileName.c_str());

	fsLocator char_fullfilename;
	fsFileUtil::ANSIFilenameToLocator( i_FileName, char_fullfilename );

	//	TODO - this GetLastName() only works if the models are in the same dir
	if (i_bIsBaseModel)
	{
		std::string just_filename = itStringUtil::GetStdString(char_fullfilename.GetLastName());
		m_BaseModel = just_filename;
	}
	//DBG_LOG3("model filenames----------------(%s) (%s)  (%s)", (i_bIsBaseModel?"True":"False"), m_BaseModel.c_str(), i_FileName.c_str());

	//add the new one
	chtrScriptData sdata;
	sdata.m_BaseData.m_Filename.SetValue( char_fullfilename.GetLastName() );
	if (i_Name.size() > 0)
	{
		sdata.m_BaseData.m_Name.SetValue(i_Name.c_str());
	}
	else
	{
		sdata.m_BaseData.m_Name.SetValue(itStringUtil::GetStdString(char_fullfilename.GetLastName()).c_str());
	}

	//	create the object
	//
	int index = chtrObjectMgr::AddObject(sdata);

	//	Create connection drivers for the object
	//
	//	Hack! - add object to the lightsets
	//
	if (index >= 0)
	{
		//	orientation
		if (!i_bIsBaseModel)
		{
			//chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(index);
			//tmlnDriverConnectChannelOrientation* pOrientDriver = dynamic_cast<tmlnDriverConnectChannelOrientation*>( CreateDriver( pAvatar, std::string("Orientation : Connect Channel")) );
			//if ( pOrientDriver )
			//{	
			//	//	set so it is the whole time
			//	tmlnDriverConnectChannelInfo* pInfo = dynamic_cast<tmlnDriverConnectChannelInfo*>( pOrientDriver->GetDriverInfo() );
			//	if ( pInfo )
			//	{
			//		pInfo->m_BeginTime = 0.0f;
			//		pInfo->m_EndTime = 999.0f;	// HACK!
			//		pInfo->m_Value = m_BaseModel;
			//	}
			//	pOrientDriver->SetDriverInfo( *pInfo );
			//	delete pInfo;
			//}
		}

	////	FullAnim
	//tmlnDriverConnectChannelFullAnim* OrientDriver = dynamic_cast<tmlnDriverConnectChannelFullAnim*>( CreateDriver( pAvatar, std::string("FullAnim : Connect Channel")) );
	//if( OrientDriver )
	//{	
	//	//	set so it is the whole time
	//	tmlnDriverFullAnimInfo* OrientInfo = dynamic_cast<tmlnDriverFullAnimInfo*>( OrientDriver->GetDriverInfo() );
	//	if( OrientInfo )
	//	{
	//		OrientInfo->m_EndTime = -1f;
	//	}
	//	OrientDriver->SetDriverInfo( *OrientInfo );
	//	delete OrientInfo;
	//}

		AddObjectToLightSets(index);
	}
	else
	{
		DBG_ERROR2("Could not add character (%s) (%s)", i_Name.c_str(), i_FileName.c_str());
	}
}

//------------------------------------------------------------------------
//	Load new characters, don't load already loaded ones, 
//	and delete ones not in this list
//------------------------------------------------------------------------
void chtrAvatarDataInterest::LoadCharacters( const std::vector<std::string>& i_Names, const std::vector<std::string>& i_FileNames, const std::vector<bool>& i_bIsBaseModels )
{
	DBG_LOG0("Load Characters");
	DBG_LOG0("===============");

	//	Build the list of character filenames in the scene.
	//
	//	NOTE: it doesn't handle duplicates well (for delete/add portion)
	//
	std::vector<std::string> current_filenames;
	std::vector<bool>	current_filenames_in_new_list;
	for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
	{
		chtrScriptObject* pObject = chtrObjectMgr::GetObjectFromIndex(i);
		fsLocator dir = pObject->GetDirectory();
		itString filename;
		pObject->GetFilename( filename );

		std::string fullpath;
		//fsFileUtil::LocatorToANSIFilename( dir, fullpath );
		//fullpath.append( "\\" );
		fullpath.append( itStringUtil::GetStdString( filename ) );
		current_filenames.push_back( fullpath );
		DBG_LOG2("%02d. In Scene (%s)", i, fullpath.c_str());
		current_filenames_in_new_list.push_back(false);
	}

	//	Load each character that isn't already in the scene
	//
	for (int i=0; i < i_FileNames.size(); ++i)
	{
		bool bAlreadyLoaded = false;
		DBG_LOG2("Loading character %02d (%s)", i, i_FileNames[i].c_str());

		//	grab just the filename
		fsLocator FnameLoc;
		fsFileUtil::ANSIFilenameToLocator( i_FileNames[i], FnameLoc );
		std::string justfname = itStringUtil::GetStdString(FnameLoc.GetLastName());

		//	search the filenames for each filename request
		for (int j=0; j < current_filenames.size(); ++j)
		{
			if (justfname == current_filenames[j])
			{
				current_filenames_in_new_list[j] = true;
				bAlreadyLoaded = true;
			}
		}

		if (!bAlreadyLoaded)
			LoadCharacter( i_Names[i], i_FileNames[i], i_bIsBaseModels[i] );
	}
	
	//	Delete characters that aren't in the i_FileNames list
	//
	for (int i=0; i < current_filenames.size(); ++i)
	{
		if (!current_filenames_in_new_list[i])
		{
			DBG_LOG2("Deleting character %02d (%s)", i, current_filenames[i].c_str());
			DeleteCharacter( current_filenames[i] );
		}
	}
}

//--------------------------------------------------------------------
//	DeleteCharacter
//--------------------------------------------------------------------
void chtrAvatarDataInterest::DeleteCharacter( const std::string& i_FileName )
{
	DBG_LOG1("Trying To Delete character (%s)", i_FileName.c_str());

	bool bFound = false;
	for (int i = chtrObjectMgr::GetNumObjects()-1; i >= 0; --i)
	{
		itString fname;
		chtrObjectMgr::GetObject(i)->GetFilename(fname);
		DBG_LOG2("    character %d (%s)", i, itStringUtil::GetStdString(fname).c_str());
		if (fname == itString(i_FileName.c_str()))
		{
			chtrObjectMgr::DeleteObject(i);

			bFound = true;
			break;
		}
	}

	//	if not found display an error
	if (!bFound)
	{
		DBG_ERROR1("Could not find (%s) to delete", i_FileName.c_str());
	}
}

//------------------------------------------------------------------------
//	replace a character model with another
//------------------------------------------------------------------------
void chtrAvatarDataInterest::ReplaceCharacter( const std::string& i_CurrentFileName, const std::string& i_NewFileName )
{
	//DBG_LOG2("Trying To replace character (%s) with (%s)", i_CurrentFileName.c_str(), i_NewFileName.c_str());

	bool bFound = false;
	for (int i = chtrObjectMgr::GetNumObjects()-1; i >= 0; --i)
	{
		itString fname;
		chtrObjectMgr::GetObject(i)->GetFilename(fname);
		//DBG_LOG2("    character %d (%s)", i, itStringUtil::GetStdString(fname).c_str());
		if (fname == itString(i_CurrentFileName.c_str()))
		{
			chtrObjectMgr::ChangeFilename(i, itString(i_NewFileName.c_str()));

			bFound = true;
			break;
		}
	}

	//	if not found display an error
	if (!bFound)
	{
		DBG_ERROR1("Could not find (%s) to replace", i_CurrentFileName.c_str());
	}
}


//------------------------------------------------------------------------
//	show or hide a character
//------------------------------------------------------------------------
void chtrAvatarDataInterest::ShowCharacter( const std::string& i_FileName, bool i_bShow )
{
	//DBG_LOG2("Trying To %s character (%s)", (i_bShow?"show":"hide"), i_FileName.c_str());

	bool bFound = false;
	for (int i = chtrObjectMgr::GetNumObjects()-1; i >= 0; --i)
	{
		itString fname;
		chtrObjectMgr::GetObject(i)->GetFilename(fname);
		//DBG_LOG2("    character %d (%s)", i, itStringUtil::GetStdString(fname).c_str());

		//	TODO - this GetLastName() won't handle full paths that are different.  Need to fix.
		fsLocator char_fullfilename;
		fsFileUtil::ANSIFilenameToLocator( i_FileName, char_fullfilename );
		itString char_filename( char_fullfilename.GetLastName() );

		if (fname == char_filename)
		{
			chtrScriptObject* pCSO = chtrObjectMgr::GetObject(i);
			if( pCSO )
			{
				pCSO->ChannelVisible().SetOriginalState( i_bShow );
				bFound = true;
				break;
			}
		}
	}

	//	if not found display an error
	if (!bFound)
	{
		DBG_ERROR1("Could not find (%s) to show or hide", i_FileName.c_str());
	}
}


//------------------------------------------------------------------------
//	Save a character as XML - the filename is the model file in the scene
//------------------------------------------------------------------------
void chtrAvatarDataInterest::SaveXMLCharacter( const std::string& i_FileName )
{
	//	TODO - implement this function.  The code below will not work.
	//
	return;

	DBG_LOG1("Trying To Save character as XML (%s)", i_FileName.c_str());

	fsLocator orig_filename, xml_filename;
	fsFileUtil::ANSIFilenameToLocator(i_FileName, orig_filename);
	xml_filename = orig_filename;
	xml_filename.ReplaceExtension( "xml" );

	bool bFound = false;
	for (int i = chtrObjectMgr::GetNumObjects()-1; i >= 0; --i)
	{
		itString fname;
		chtrObjectMgr::GetObject(i)->GetFilename(fname);
		DBG_LOG2("    character %d (%s)", i, itStringUtil::GetStdString(fname).c_str());
		if (fname == orig_filename.GetLastName())
		{
			chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);
			pObject->SaveMaterials(xml_filename, gfFileConstants::eFileXML);

			bFound = true;
			break;
		}
	}

	//	if not found display an error
	if (!bFound)
	{
		DBG_ERROR1("Could not find (%s) to save as XML", i_FileName.c_str());
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrAvatarDataInterest::AvatarChanged()
{
	////	Read the parts data in 
	////
	//	orthoAvatarDataUtil::ReadData( std::string("Avatar1-Parts.txt") );

	////	Read the anim data in 
	////
	//orthoAnimDataUtil::ReadData( std::string("Avatar1-Anims.txt"), orthoAnimDataUtil::Data() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriver* chtrAvatarDataInterest::CreateDriver( chtrScriptObject* pAvatar, const std::string& i_driverName )
{
	//
	//	create the driver
	//
	if( pAvatar )
	{
		// get the drivers and find if the driver exists
		//
		tmlnDriverNameList driver_names;
		tmlnCreator::GatherPossibleDrivers( pAvatar, driver_names);

		if( !driver_names.Empty())
		{
			//	find the index
			//
			int i, num_drivers;
			num_drivers = driver_names.m_DriverNames.size();
			DBG_TRACE("About to compare " << num_drivers << " drivers");
			for (i=0; i< num_drivers; i++)
			{
				DBG_TRACE( i << " " << driver_names.m_DriverNames[i].m_Name.c_str() << " vs " << i_driverName.c_str() );
				if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),i_driverName.c_str()) == 0)
				{
					DBG_TRACE("----found");
					break;
				}
			}

			if ( i >= driver_names.m_DriverNames.size() )
			{
				return( NULL );
			}

			tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

			// Create Driver
			//
			tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( 
				i_driverName.c_str(), pAvatar );

			// Give driver to script object to own
			if (pDriver)
			{
				pAvatar->AddDriver( pDriver );
				pAvatar->NotifyDriverChanged();

				return( pDriver );
			}
		}
	}
	return( NULL );
}

//--------------------------------------------------------------------
// Adds an animation sequence to the interest
//--------------------------------------------------------------------
void chtrAvatarDataInterest::AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras )
{
	float curTime = 0;

	//add to each object
	for( int o = 0; o < chtrObjectMgr::GetNumObjects(); o++ )
	{
		curTime = 0;

		//search for end of animation channel
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(o);
		if( pAvatar )
		{
			//	only add the drivers for the base model, the rest should have connection drivers.
			//
			fsLocator Av_filename = pAvatar->GetBaseData().m_Filename.GetValue();
			std::string filename;
			fsFileUtil::LocatorToANSIFilename( Av_filename, filename );

			//	NOTE - put this one back if we get a connect driver for full anim
			//if (m_BaseModel == filename)
			{
				//DBG_LOG2("Adding anim to char %d (%s)", o, i_AnimFile.c_str());

				tmlnChannelAnimationFull& animchannel = pAvatar->ChannelAnimationFull();
				for( int i = 0; i < animchannel.GetNumDrivers(); i++ )
				{
					float time = animchannel.GetDriver( i ).GetEndTime();
					if ( time > curTime ) 
						curTime = time;
				}
				//tmlnTimeLine::SetValue( ceil( curTime ) );	//increment to next whole time value
				tmlnTimeLine::SetValue( curTime );	//increment to end of existing animation

				OrthoAvatarTemporalSpace::animStartTime = tmlnTimeLine::GetValue();	//save start

				int count = i_Divisions;
				if( count < 1 ) count = 1;
				double divisor = maConstants::c_dPI_Times_2/count;

				//	Create an animation driver that spans the count
				//
				float single_anim_duration = 0.0f;
				tmlnDriverAnimationFull* animDriver = dynamic_cast<tmlnDriverAnimationFull*>( CreateDriver( pAvatar, std::string("Anim-Full")) );
				if ( animDriver )
				{	
					//load the animation into this driver
					try
					{
						animDriver->load_animation( i_AnimFile );
					}
					catch( fsFileDoesntExistX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + i_AnimFile;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File does not exist");
						return;  //terminate and return immediately
					}


					tmlnDriverAnimationFullInfo* pInfo = dynamic_cast<tmlnDriverAnimationFullInfo*>(animDriver->GetDriverInfo());
					if (pInfo)
					{
						single_anim_duration = animDriver->GetAnimationLength();
						single_anim_duration += 1.0f / pInfo->m_Info.m_FrameRate;	//add in additional frame for looped animation.

						//	for the first character, store the values so some outside entity can
						//	get this information.
						//
						if (m_BaseModel == filename)
						{
							for (int j = 0; j < count; ++j)
							{
								int index = orthoRemoteCommandMgr::m_AnimFrameData.size();
								orthoRemoteCommandMgr::m_AnimFrameData.resize(index+1);
								orthoRemoteCommandMgr::m_AnimFrameData[index].m_AnimFile = i_AnimFile;
								orthoRemoteCommandMgr::m_AnimFrameData[index].m_fAnimStartTime = curTime + (single_anim_duration * j);
								orthoRemoteCommandMgr::m_AnimFrameData[index].m_fAnimLength = single_anim_duration;
							}
						}

						pInfo->m_Info.m_bDriverToAnimLength = false;
						pInfo->m_Info.m_bLooping = true;
						animDriver->SetDriverInfo(*pInfo);
						animDriver->SetEndTime( (count * single_anim_duration) );
						delete pInfo;
					}
				}

				//
				//	don't add orientation to all, just base
				//
				//DBG_LOG2("model filenames----------------(%s)  (%s)", m_BaseModel.c_str(), filename.c_str());
				//if (m_BaseModel == filename)
				{
					for ( int i = 0; i < count; ++i )
					{
						//DBG_LOG2("Creating Orientation Driver #%d at time %6.3f", i, tmlnTimeLine::GetValue());

						//create an orientation driver at the current time
						tmlnDriverOrientation* OrientDriver = dynamic_cast<tmlnDriverOrientation*>( CreateDriver( pAvatar, std::string("Static Orientation")) );
						if( OrientDriver )
						{	//set the rotation based on the divisions
							tmlnDriverOrientationInfo* OrientInfo = dynamic_cast<tmlnDriverOrientationInfo*>( OrientDriver->GetDriverInfo() );
							if( OrientInfo )
							{
								OrientInfo->m_Y = (float)(i * divisor);
							}
							OrientDriver->SetDriverInfo( *OrientInfo );

							//	for the first character, store the values so some outside entity can
							//	get this information.
							//
							if( i < orthoRemoteCommandMgr::m_AnimFrameData.size() )
							{
								orthoRemoteCommandMgr::m_AnimFrameData[i].m_OrientationY = OrientInfo->m_Y;
							}
							delete OrientInfo;

							curTime += single_anim_duration;
							tmlnTimeLine::SetValue( curTime );			//increment to right after the animation
						}
					}

					if (m_BaseModel == filename)
						OrthoAvatarTemporalSpace::animEndTime = curTime;	//save end
				}
			}
		}
	}
	tmlnTimeLine::SetTimeRange( 0.0f, OrthoAvatarTemporalSpace::animEndTime );

	// debug only
	//if (0)
	//{
	//	for (int k = 0; k < orthoRemoteCommandMgr::m_AnimFrameData.size(); ++k)
	//	{
	//		DBG_LOG5("%02d time(%6.3f) len(%6.3f) orient(%6.3f) file(%s)", 
	//			k, 
	//			orthoRemoteCommandMgr::m_AnimFrameData[k].m_fAnimStartTime,
	//			orthoRemoteCommandMgr::m_AnimFrameData[k].m_fAnimLength,
	//			orthoRemoteCommandMgr::m_AnimFrameData[k].m_OrientationY,
	//			orthoRemoteCommandMgr::m_AnimFrameData[k].m_AnimFile.c_str() );
	//	}
	//}
}

//--------------------------------------------------------------------
//	Add Expression
//--------------------------------------------------------------------
void chtrAvatarDataInterest::AddExpression(const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue)
{
	//
	//	1) figure out which object
	//	2) delete expressions (only if not same expression
	//	3) add expression to object
	//	4) add single driver with value
	//

	float curTime = 0;

	//	add to the correct object
	for( int o = 0; o < chtrObjectMgr::GetNumObjects(); o++ )
	{
		curTime = 0;

		//search for avatar
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(o);
		if( pAvatar )
		{
			itString filename;
			pAvatar->GetFilename(filename);
			if (   (pAvatar->GetName() == nameString(i_ObjectName))
				|| (filename == itString(i_ObjectName.c_str())) )
			{
				chtrExpressionObject* pEO = pAvatar->ExpressionObject();
				if (pEO != NULL)
				{
					//	see if the expression is already loaded.
					bool bFound = false;
					int num_exp = pEO->GetNumExpressions();
					for (int e=0; e < num_exp; ++e)
					{
						//if (pEO->GetFileName(e).GetLastName() == i_FileName.GetLastName())
						//	bFound = true;
					}
					//if (!bFound)
						pEO->DeleteExpressions();

					//	add the new expression
					pAvatar->AddSingleExpression(i_ExpressionName, i_ExpressionFileName);

					//	add a driver with the expression value
					std::string driver_name("Expression Expression");
					//driver_name.append(i_ExpressionName);
					tmlnDriverFloat* pDriver = dynamic_cast<tmlnDriverFloat*>( CreateDriver( pAvatar, driver_name ) );
					if ( pDriver != NULL )
					{
						pDriver->SetValue(i_ExpressionValue);
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//	Delete All Drivers
//--------------------------------------------------------------------
void chtrAvatarDataInterest::DeleteSceneDrivers()
{
	//delete from each object
	//
	for( int o = 0; o < chtrObjectMgr::GetNumObjects(); o++ )
	{
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(o);
		if( pAvatar )
		{
			//	only delete the drivers from the base model
			//
			fsLocator Av_filename = pAvatar->GetBaseData().m_Filename.GetValue();
			std::string filename;
			fsFileUtil::LocatorToANSIFilename( Av_filename, filename );
			//if (m_BaseModel == filename)
			{
				int count = pAvatar->GetNumDrivers();
				while( count > 0 )
				{
					tmlnDriver* drv = pAvatar->GetDriverDirect(count-1);
				
					tmlnDriverAnimationFull* animdrv = dynamic_cast<tmlnDriverAnimationFull*>(drv);
					tmlnDriverOrientation* ordrv = dynamic_cast<tmlnDriverOrientation*>(drv);
					if( animdrv )	//test for animation driver
					{
						pAvatar->RemoveDriver( animdrv );
						delete animdrv;
						//DBG_LOG1("Clearing animation driver for char %s", pAvatar->GetTmlnName().c_str());
					}
					else if( ordrv )	//test for orientation driver
					{
						pAvatar->RemoveDriver( ordrv );
						delete ordrv;
						//DBG_LOG1("Clearing Orientation driver for char %s", pAvatar->GetTmlnName().c_str());
					}
					count--;
				}
			}
		}
	}

	//
	//
	orthoRemoteCommandMgr::m_AnimFrameData.clear();
}

//--------------------------------------------------------------------
//	PartChanged
//--------------------------------------------------------------------
void chtrAvatarDataInterest::PartChanged(const std::string i_JointName, bool i_bVisible )
{
	//	find the animation and set it
	//
	for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
	{
		chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);

		if (pObject != NULL)
		{
#define USE_JOINT_METHOD
#ifdef USE_JOINT_METHOD
			//	Joint/Node method of turning on/off parts of a model.
			//	Note: this doesn't work with animation
			//
			{
				entEntity* pEntity = pObject->GetPickObject()->GetEntity()->GetEntity();
				g3dSceneNode* pNode = pEntity->GetNamedNode( i_JointName.c_str() );

				if (pNode != NULL)
				{
					pNode->SetRenderable( i_bVisible );
				}
			}
#else
			//	Fragment method of turning on/off parts of a model.
			//	Note: 
			//
			{
				//scObject* pScObject = 
				//g3dSceneNode* pBase = pScObject->GetBase();
				//g3dSceneNode* pNode = pBase->GetNamedNode( i_JointName.c_str() );

				chtrScriptData data;
				pObject->GetFragmentData(data.m_Fragments);
				for (int fi = 0; fi < data.m_Fragments.size(); ++fi)
				{
					data.m_Fragments[fi].SetShadowHull(i_bVisible);
					data.m_Fragments[fi].SetCastsShadow(i_bVisible);
				}
			}
#endif
		}
	}
}


//--------------------------------------------------------------------
//	MaterialChanged
//--------------------------------------------------------------------
void chtrAvatarDataInterest::MaterialChanged(const std::string i_PartName, const maFloatRGBA& i_Color)
{
	bool bFound = false;

	//	find the animation and set it
	//
	for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
	{
		chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);

		if (pObject != NULL)
		{
			// mtrlScriptObject interface.
			//	call GatherMaterials first.
			//	GetIndexForName(), then get/set materialdata
			//
#define CHTR_PROPER_MATERIALCHANGE_METHOD
#ifdef CHTR_PROPER_MATERIALCHANGE_METHOD
			if (pObject->GetNumMaterials() == 0)
				pObject->GatherMaterials();
			int index = pObject->mtrlScriptObject::GetIndexForName(i_PartName);
			if (index != -1)
			{
				bFound = true;

				mdlMaterialInfo MatInfo = pObject->GetMaterialData(index);

				//	Phong Data
				shared_ptr<effShaderParams> pData = MatInfo.GetShaderParams();
				itString effectName = pData->GetShaderName();

				if( effectName == itString("PhongBump.fx") || 
					effectName == itString("Phong.fx") || 
					effectName == itString("Simple.fx") )
				{
					//	this is overriding the material library so clear out the filename
					MatInfo.SetLibraryFilename(fsLocator());

					//	set the color
					effParamColor* pColor = pData->FindColorParam("g_diffuse");
					if (pColor)
					{
						pColor->Property().SetValue(i_Color);
					}
				}
				else
				{
					//	Hair Data
					//
					//	Note: the r,g,b,a values are the "original" for the male test model.
					//
					if (effectName == itString("Hair.fx"))
					{
						//	this is overriding the material library so clear out the filename
						MatInfo.SetLibraryFilename(fsLocator());

						effParamColor* pColor = pData->FindColorParam("g_hairBaseColor");
						if (pColor)
						{
							pColor->Property().SetValue(i_Color);
						}
						pColor = pData->FindColorParam("g_specularColor0");
						if (pColor)
						{
							pColor->Property().SetValue(i_Color);
						}
					}
					else
					{
						//	Skin Data
						if (effectName == itString("Skin.fx"))
						{
							//	this is overriding the material library so clear out the filename
							MatInfo.SetLibraryFilename(fsLocator());

							effParamColor* pColor = pData->FindColorParam("g_transColIn");
							if (pColor)
							{
								pColor->Property().SetValue(i_Color);
							}
							pColor = pData->FindColorParam("g_transColOut");
							if (pColor)
							{
								pColor->Property().SetValue(i_Color);
							}
							pColor = pData->FindColorParam("g_transColBack");
							if (pColor)
							{
								pColor->Property().SetValue(i_Color);
							}
						}
					}
				}
				pObject->ChangeMaterialData(index, MatInfo, false);
			}
#else
			entEntity* pEntity = pObject->GetPickObject()->GetEntity()->GetEntity();
			g3dSceneNode* pNode = pEntity->GetNamedNode( i_PartName.c_str() );

			if (pNode != NULL)
			{
//				m_pMaterial->TypedData<effPhongData>()->m_ColorDiffuse = i_Color;
				g3dFragment* pFrag = pNode->GetFragment();
				if (pFrag != NULL)
				{
					matMaterial* pMat = pFrag->GetMaterial();
					if (pMat != NULL)
					{
						effPhongData* pEffData = dynamic_cast<effPhongData*>(pMat->GetEffectData());
						if (pEffData != NULL)
						{
							pEffData->m_ColorDiffuse = i_Color;
						}
					}
				}
			}
#endif
		}
	}
	if( !bFound )
	{
		DBG_ERROR1("Material %s not found on objects", i_PartName.c_str() );
	}
}


//------------------------------------------------------------------------
//	notify interests a part gets attached/removed
//------------------------------------------------------------------------
void chtrAvatarDataInterest::AttachPart( const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach)
{
	//	find the animation and set it
	//
	bool bFound = false;
	int num_chars = chtrObjectMgr::GetNumObjects();
	for (int i=0; i < num_chars; ++i)
	{
		//	this should be the character
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(i);

		if (pAvatar != NULL)
		{
			itString fname;
			pAvatar->GetFilename(fname);

			//	verify the name or filename is the same because multiple models have the same joint names.
			//
			if (   (pAvatar->GetName() == nameString(i_Name))
				|| (fname == itString(i_Name.c_str())) )
			{
				std::vector<std::string> ref_list;
				pAvatar->GetReferenceList(ref_list);
				if (envSTLHelpers::Contains(ref_list, i_PartName))
				{
					if (i_bAttach)
					{
						//	create the data structure with the object to attach (i.e. "glasses") and the part on
						//	the other model to attach to (i.e. "nose_bridge" of the character)
						//
						chtrScriptData sdata;
						fsLocator attach_filename;
						fsFileUtil::ANSIFilenameToLocator(i_AttachFile, attach_filename);

						//	TODO - switch this so it can recognize the full path
						//
						sdata.m_BaseData.m_Filename.SetValue( attach_filename.GetLastName() );
						sdata.m_BaseData.m_Name.SetValue( itStringUtil::GetStdString( attach_filename.GetLastName() ).c_str() );

						//	add the attach object if it isn't already there.
						//
						chtrScriptObject* pAttachObj = NULL;
						for (int i = 0; i < chtrObjectMgr::GetNumObjects(); ++i)
						{
							pAttachObj = chtrObjectMgr::GetObject(i);
							if (pAttachObj != NULL)
							{
								if (pAttachObj->GetName() == nameString(i_AttachFile))
								{
									break;
								}
								else
									pAttachObj = NULL;
							}
						}

						//	Object didn't exist yet, so add it
						//
						//  Note: This will NOT allow duplicate objects to be in the world.
						//		Do we want this?  This would cause only one earring to load.
						//
						if (pAttachObj == NULL)
						{
							int chtr_index = chtrObjectMgr::AddObject(sdata);
							if( chtr_index < 0 ) return;
							pAttachObj = chtrObjectMgr::GetObject(chtr_index);

							//	Hack! - add object to the lightsets
							AddObjectToLightSets(chtr_index);
						}

						//	create the info object for attaching
						chDefs::Name ChunkName = chDefs::MakeName('P', 'A', 'T', 'O');
						tmlnDriverAttachOrientInfo info(ChunkName);
						info.m_AttachName = i_PartName;
						info.m_ObjectName = pAvatar->GetName(); //.SetString( i_AttachFile );
						tmlnDriver* pDriver = tmlnCreator::CreateDriverFromInfo( pAttachObj, &info ); //pAvatar, &info );

						tmlnDriverAttachOrient* pADriver = dynamic_cast<tmlnDriverAttachOrient*>(pDriver);
						//if (pADriver != NULL)
						//{
						//}
						bFound = true;
					}
					else
					{
						fsLocator attach_file;
						fsFileUtil::ANSIFilenameToLocator( i_AttachFile, attach_file );
						std::string file_name_only;
						file_name_only = itStringUtil::GetStdString( attach_file.GetLastName() );

						//	find the attach object
						//
						chtrScriptObject* pAttachObj = NULL;
						for (int i = 0; i < chtrObjectMgr::GetNumObjects(); ++i)
						{
							pAttachObj = chtrObjectMgr::GetObject(i);
							if (pAttachObj != NULL)
							{
								if (pAttachObj->GetName() == nameString(file_name_only))
								{
									//	found it, now remove it.
									chtrObjectMgr::DeleteObject(i);
									return;
								}
							}
						}
					}
				}
			}
		}
	}
	if( !bFound )
	{
		DBG_WARNING2( "AttachPart %s not found with object %s or object doesn't exist.", i_PartName.c_str(), i_Name.c_str() );
	}
}


//------------------------------------------------------------------------
//	change the texture of a material
//------------------------------------------------------------------------
void chtrAvatarDataInterest::ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName)
{
	bool bFound = false;

	//supported layer strings (case-insensitive)
	//Diffuse/Base = 0
	//Normal = 1
	//Specular = 2
	//SpecularPower = 3
	//Transparent/Transparency/Alpha = 4
	//Translucent/Translucency = 5
	//Gloss = 6
	//SpecularMask = 7
	//SpecularShift = 8
	//Micro = 9
	//Environment = 10

	//map to ID
	int ID = -1;
	if( _stricmp( i_TextureLayer.c_str(), "diffuse" ) == 0 ) ID = 0;
	else if( _stricmp( i_TextureLayer.c_str(), "base" ) == 0 ) ID = 0;
	else if( _stricmp( i_TextureLayer.c_str(), "normal" ) == 0 ) ID = 1;
	else if( _stricmp( i_TextureLayer.c_str(), "specular" ) == 0 ) ID = 2;
	else if( _stricmp( i_TextureLayer.c_str(), "specularpower" ) == 0 ) ID = 3;
	else if( _stricmp( i_TextureLayer.c_str(), "transparent" ) == 0 ) ID = 4;
	else if( _stricmp( i_TextureLayer.c_str(), "transparency" ) == 0 ) ID = 4;
	else if( _stricmp( i_TextureLayer.c_str(), "alpha" ) == 0 ) ID = 4;
	else if( _stricmp( i_TextureLayer.c_str(), "translucent" ) == 0 ) ID = 5;
	else if( _stricmp( i_TextureLayer.c_str(), "translucency" ) == 0 ) ID = 5;
	else if( _stricmp( i_TextureLayer.c_str(), "gloss" ) == 0 ) ID = 6;
	else if( _stricmp( i_TextureLayer.c_str(), "specularmask" ) == 0 ) ID = 7;
	else if( _stricmp( i_TextureLayer.c_str(), "specularshift" ) == 0 ) ID = 8;
	else if( _stricmp( i_TextureLayer.c_str(), "micro" ) == 0 ) ID = 9;
	else if( _stricmp( i_TextureLayer.c_str(), "environment" ) == 0 ) ID = 10;

	if( ID == -1 )
	{
		DBG_WARNING1("Unsupported Layer %s, defaulting to diffuse", i_TextureLayer.c_str() );
		ID = 0;
	}

	//	find the animation and set it
	//
	for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
	{
		chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);

		if (pObject != NULL)
		{
			// mtrlScriptObject interface.
			//	call GatherMaterials first.
			//	GetIndexForName(), then get/set materialdata
			//
			if (pObject->GetNumMaterials() == 0)
				pObject->GatherMaterials();
			int index = pObject->mtrlScriptObject::GetIndexForName(i_MaterialName);
			if (index != -1)
			{
				bFound = true;
				mdlMaterialInfo MatInfo = pObject->GetMaterialData(index);

				//override material
				shared_ptr<effShaderParams> pData = MatInfo.GetShaderParams();
				itString effectName = pData->GetShaderName();
				if( effectName == itString("PhongBump.fx") || 
					effectName == itString("Phong.fx") || 
					effectName == itString("Simple.fx") )
				{
					switch( ID )
					{
						case 0:		SetMaterialTexture(pData, "diffuseMap", i_TextureName); break;
						case 1:		SetMaterialTexture(pData, "normalMap", i_TextureName); break;
						case 2:		SetMaterialTexture(pData, "specularMap", i_TextureName); break;
						case 4:		SetMaterialTexture(pData, "transparencyMap", i_TextureName); break;
						case 6:		SetMaterialTexture(pData, "glossMap", i_TextureName); break;
						case 10:	SetMaterialTexture(pData, "cubeMap", i_TextureName); break;
					}
				}
				else if( effectName == itString("Hair.fx") )
				{
					switch( ID )
					{
						case 0:	SetMaterialTexture(pData, "tBase", i_TextureName); break;
						case 1:	SetMaterialTexture(pData, "tNormalMap", i_TextureName); break;
						case 4:	SetMaterialTexture(pData, "tAlpha", i_TextureName); break;
						case 7: SetMaterialTexture(pData, "tSpecularMask", i_TextureName); break;
						case 8:	SetMaterialTexture(pData, "tSpecularShift", i_TextureName); break;
					}
				}
				else if( effectName == itString("Skin.fx") )
				{
					switch( ID )
					{
						case 0:	SetMaterialTexture(pData, "diffTex", i_TextureName); break;
						case 1:	SetMaterialTexture(pData, "normalTex", i_TextureName); break;
						case 2:	SetMaterialTexture(pData, "specTex", i_TextureName); break;
						case 3:	SetMaterialTexture(pData, "specPowerTex", i_TextureName); break;
						case 4:	SetMaterialTexture(pData, "transparencyTex", i_TextureName); break;
						case 5:	SetMaterialTexture(pData, "transTex", i_TextureName); break;
						case 9:	SetMaterialTexture(pData, "microTex", i_TextureName); break;
					}
				}
				else continue;	//no valid material to override, so go onto the next

				//	Apply the material changes
				//
				try
				{
					MatInfo.SetLibraryFilename(fsLocator());
					pObject->ChangeMaterialData(index, MatInfo, false);
				}
				catch( fsFileDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					DBG_ERROR1("Cannot read file %s", filename.c_str() );
					guiMessageBox::Show( filename.c_str(), "File does not exist");
					return;
				}
			}
		}
	}
	if( !bFound )
	{
		DBG_ERROR1("Material %s not found on objects", i_MaterialName.c_str() );
	}
}



//--------------------------------------------------------------------
//	ChangeMaterial
//--------------------------------------------------------------------
void chtrAvatarDataInterest::ChangeMaterial(const std::string i_PartName, const std::string& i_MaterialFile)
{
	int index;
	bool bFound = false;
	bool bReset = false;
	bool bAll = false;
	const mdlMaterialInfo* defMtl = NULL;

	//  find if material name exists in defaults
	//
	std::map< std::string, mdlMaterialInfo >::iterator dm_iter = m_DefaultMaterials.find( i_PartName );
	if( dm_iter != m_DefaultMaterials.end() )
	{
		defMtl = &dm_iter->second;
	}

	if( _stricmp( i_MaterialFile.c_str(), "default" ) == 0 )	//are we resetting to defaults
	{
		bReset = true;
		if( _stricmp( i_PartName.c_str(), "all"	) == 0 )	//and if all materials
		{
			bAll = true;

			std::map< std::string, mdlMaterialInfo >::const_iterator dm_iter = m_DefaultMaterials.begin();
			for( ; dm_iter != m_DefaultMaterials.end(); dm_iter++ )	//reset all default materials
			{
				//search all objects for matching material
				for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
				{
					chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);

					if (pObject != NULL)
					{
						if (pObject->GetNumMaterials() == 0)
							pObject->GatherMaterials();
						int index = pObject->mtrlScriptObject::GetIndexForName( dm_iter->first );
						if (index >= 0 )	//material found
						{
							std::string temppath;
							fsFileUtil::LocatorToANSIFilename( dm_iter->second.GetLibraryFilename(), temppath);

							//DBG_LOG2("Default material %s restored on %s", temppath.c_str(), dm_iter->second.GetMaterialName().c_str() );
							pObject->ChangeMaterialData(index, dm_iter->second, true );
						}
					}
				}
			}
			return;
		}
	}

	//	find the animation and set it
	//
	for (int i=0; i < chtrObjectMgr::GetNumObjects(); ++i)
	{
		chtrScriptObject* pObject = chtrObjectMgr::GetObject(i);

		if (pObject != NULL)
		{
			if (pObject->GetNumMaterials() == 0) pObject->GatherMaterials();

			index = pObject->mtrlScriptObject::GetIndexForName( i_PartName );
			if (index >= 0 )
			{
				bFound = true;

				if( !defMtl )	//no default material so save it away
				{
					m_DefaultMaterials[ i_PartName ] = pObject->GetMaterialData(index);
				}
				else if( bReset )	//are we resetting, use existing default material
				{
					std::string temppath;
					fsFileUtil::LocatorToANSIFilename( defMtl->GetLibraryFilename(), temppath);

					//DBG_LOG2("Default material %s restored on %s", temppath.c_str(), defMtl->GetMaterialName().c_str() );
					pObject->ChangeMaterialData(index, *defMtl, true );
				}

				if( !bReset )
				{
					std::string fname;

					const fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
					//fsFileUtil::LocatorToANSIFilename(init_dir, fname);
					fsLocator relative_path, full_path;
					fsFileUtil::ANSIFilenameToLocator(i_MaterialFile, relative_path);
					//fsFileUtil::LocatorToANSIFilename(init_dir, fname);
					//relative_path.RemoveBefore(init_dir.GetNumNames());
					fsFileUtil::LocatorToANSIFilename(init_dir, fname);
					full_path = init_dir;
					full_path.Push(relative_path);

					std::string temppath;
					fsFileUtil::LocatorToANSIFilename(full_path, temppath);
					DBG_LOG1("Material relative path = (%s)", temppath.c_str());

					try
					{
						mdlMaterialInfo mat_info;
						if (mtrMaterialSaver::ReadSingleMaterial(full_path, mat_info))
						{
							// Set relative path to the library file we just read
							// into material info so that it can be used to fetch
							// the textures for this material
							mat_info.SetLibraryFilename( relative_path );
							const bool bUpdateProperties = true; 
							pObject->ChangeMaterialData(index, mat_info, bUpdateProperties);
						}
					}
					catch( const fsInvalidLocatorX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File does not exist");
					}
					catch( const fsFileDoesntExistX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File does not exist");
					}
					catch( const fsUnknownX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File can not be loaded");
					}
				}
			}
		}
	}
	if( !bFound )
	{
		DBG_ERROR1("Material %s not found on objects", i_PartName.c_str() );
	}
}

//--------------------------------------------------------------------
//	Add an object to lightsets
//--------------------------------------------------------------------
void chtrAvatarDataInterest::AddObjectToLightSets(int i_ObjectIndex)
{
	// TODO - figure out a better way to do this.
	//
	// HACK - this is a complete hack to automatically put the new object into a lightset.
	//
	chtrScriptObject* pSO = chtrObjectMgr::GetObject( i_ObjectIndex );
	if (pSO != NULL)
	{
		lsetOperations::AddObjectToLightSet(nameString("Character"), pSO->GetName());
		if (pSO->GetName().GetString().find_first_of("Body") != string::npos)
		{
			for (int i=0; i< pSO->GetNumFragments(); ++i)
			{
				if (   (pSO->GetFragmentName(i).find_first_of("Iris") != string::npos)
					|| (pSO->GetFragmentName(i).find_first_of("Lens") != string::npos))
				{
					lsetOperations::AddNodeToLightSet(nameString("Character"), pSO->GetName(), i);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//	ScaleCharacter
//--------------------------------------------------------------------
void chtrAvatarDataInterest::ScaleCharacter( const maVector3d& i_Scale )
{
	for( int o = 0; o < chtrObjectMgr::GetNumObjects(); o++ )
	{
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(o);
		if( pAvatar )
		{
			pAvatar->ChannelScale().SetValue( i_Scale.GetX() );
		}
	}
}

//--------------------------------------------------------------------
//	RotateCharacter
//--------------------------------------------------------------------
void chtrAvatarDataInterest::RotateCharacter( float angle )
{
	for( int o = 0; o < chtrObjectMgr::GetNumObjects(); o++ )
	{
		chtrScriptObject* pAvatar = chtrObjectMgr::GetObject(o);
		if( pAvatar )
		{
			tmlnChannelOrientation& Orient = pAvatar->ChannelOrientation();

			if( Orient.GetNumDrivers() >= 1 )	//have at least one driver so set the first's orientation
			{
				tmlnDriverOrientation* pOrientDriver = dynamic_cast<tmlnDriverOrientation*>(&Orient.Driver(0));
				if( pOrientDriver )
				{
					tmlnDriverOrientationInfo* OrientInfo = dynamic_cast<tmlnDriverOrientationInfo*>( pOrientDriver->GetDriverInfo() );
					if( OrientInfo )
					{
						OrientInfo->m_Y = angle;
					}
					pOrientDriver->SetDriverInfo( *OrientInfo );
					delete OrientInfo;
				}
			}
			else
			{
				Orient.SetOriginalValue( 0, angle, 0 );
			}
		}
	}
}


