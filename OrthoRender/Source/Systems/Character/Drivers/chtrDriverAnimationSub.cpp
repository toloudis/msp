/*****************************************************************************
**	chtrDriverAnimationSub.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Drivers/chtrDriverAnimationSub.hpp"

#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/Timeline/chtrChannelAnimationSub.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationSubInfo.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationSubParser.hpp"
//#include "Systems/Character/Drivers/chtrDriverAnimationSubForm.h"

#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimKeys.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Core/it/itStringUtil.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationSub::chtrDriverAnimationSub(chtrChannelAnimationSub &i_Channel,
											   const fsLocator &i_CharacterDir)
:	m_Channel(i_Channel), 
	m_pAnimation(NULL),
	m_pAnimKeys(NULL),
	m_CharacterDir(i_CharacterDir),
	m_bDriverToAnimLength("Driver to Anim Length",true),
	m_AnimFilename("Animation File Name"),
	m_AnimName("Anim Name"),
	m_FrameRate("Frame Rate",g3dConstants::c_fDefaultFrameRate),
	m_StartFrame("Start Frame",-1),			// -1.0 is default / not set
	m_EndFrame("End Frame",-1),				// -1.0 is default / not set
	m_bLooping("Looping",false)
{
	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyPropertyUIInfo(&(m_bDriverToAnimLength), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_AnimFilename), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_AnimName), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_FrameRate), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_StartFrame), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_EndFrame), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_bLooping), "category", "description");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_AnimFilename.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_AnimName.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_StartFrame.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_EndFrame.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<chtrDriverAnimationSub>(this, &chtrDriverAnimationSub::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationSub::~chtrDriverAnimationSub()
{
//	DBG_LOG0("chtrDriverAnimationSub: destroying animation");
	delete m_pAnimation;
	delete m_pAnimKeys;
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  chtrDriverAnimationSub::Operate(float i_Time)
{
	if (m_pAnimation)
	{
		// Check and handle blend into driver
		if (IsBefore(i_Time))
		{
			//float blend_time = 0.0f;
			//if (get_blend_time(i_Time, blend_time))
			//{
			//	m_Channel.BlendAnimation( m_AnimIndex, this->GetBeginTime(), blend_time);
			//}
		}
		else
		{
			m_Channel.AddSubAnimation( m_pAnimation, this->GetBeginTime() );
		}
	}
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  chtrDriverAnimationSub::GetDriverInfo() const
{
	chtrDriverAnimationSubInfo *pInfo = new chtrDriverAnimationSubInfo(chtrDriverAnimationSubParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Info.m_AnimFilename = m_AnimFilename.GetValue();
	pInfo->m_Info.m_AnimName = m_AnimName.GetValue();
	pInfo->m_Info.m_bDriverToAnimLength = m_bDriverToAnimLength.GetValue();

	pInfo->m_Info.m_FrameRate = m_FrameRate.GetValue();
	pInfo->m_Info.m_bLooping = m_bLooping.GetValue();
	pInfo->m_Info.m_StartFrame = m_StartFrame.GetValue();
	pInfo->m_Info.m_EndFrame = m_EndFrame.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetDriverInfo(	const chtrDriverAnimationSubInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// this will load and create the animation as needed
	SetAnimFilename( i_Info.m_Info.m_AnimFilename, i_Undoable );

	this->SetAnimName( i_Info.m_Info.m_AnimName, i_Undoable );
	this->SetDriverToAnimLengthFlag( i_Info.m_Info.m_bDriverToAnimLength, i_Undoable );
	this->SetFrameRate( i_Info.m_Info.m_FrameRate, i_Undoable );
	this->SetLooping( i_Info.m_Info.m_bLooping, i_Undoable );
	this->SetStartFrame( i_Info.m_Info.m_StartFrame, i_Undoable );
	this->SetEndFrame( i_Info.m_Info.m_EndFrame, i_Undoable );
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
//void  chtrDriverAnimationSub::DoEditProperties()
//{
//#ifdef _MANAGED
//	//StudioFramework::
//	SystemChtr::chtrDriverAnimationSubForm ^ form =
//		gcnew SystemChtr::chtrDriverAnimationSubForm( *this );
//
////	form->Show();
//	tmaDialogTabbedMgr::RemoveTabPages("Driver");
//	tmaDialogTabbedMgr::AddTabPage("Driver", form->GetTabPage(0));
//
//	tmlnDriver::AddBasePropertiesTab();
//
//	tmaDialogTabbedMgr::Show("Driver");
//
//	// The tab page from our custom form is now in the general
//	// driver properties dialog, we can delete the custom form.
//	delete form;
//#endif
//}


//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float chtrDriverAnimationSub::GetAnimationNumFrames() const
{
	if (m_pAnimation)
	{
		return m_pAnimation->GetNumFrames();
	}
	return 0;
}

//--------------------------------------------------------------------
// Returns how long the animation is in seconds given the 
// frame rate and other data about this animation.
//--------------------------------------------------------------------
float chtrDriverAnimationSub::GetAnimationLength() const
{
	if (m_pAnimation)
	{
		return m_pAnimation->GetLength();
	}
	return 0;
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void chtrDriverAnimationSub::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->m_AnimFilename.GetValue() );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetAnimFilename( const itString& i_AnimFilename, 
											 prtyProperty::UndoFlags i_Undoable)
{
	if (m_AnimFilename.GetValue() != i_AnimFilename)
	{
		// Clear out animation in channel so that it doesn't use the animation
		// we are about to delete anymore.
		m_Channel.Reset();

		delete m_pAnimation;
		delete m_pAnimKeys;
		m_pAnimKeys = NULL;

		m_AnimFilename.SetValue(i_AnimFilename, i_Undoable);

		//	find all the paths for this path index and use each one of these as the
		//	base for the full path.
		//
		fsLocator anim_loc;
		int numpaths = gfPaths::GetNumPathsInList( mnmPaths::e_DataSceneAndStock );
		for ( int i = 0 ; i < numpaths && !m_pAnimKeys; ++i )
		{
			//	generate the list of child directories for this path
			//
			anim_loc = gfPaths::GetPath(mnmPaths::e_DataSceneAndStock, i);
			itString sdir = chtrAnimList::GetSystemDirName();
			anim_loc.Push( sdir );

			//	for each child directory append the data directory name, 
			//	get all the files of the appropriate extension, and add them
			//	to the list.
			//
			std::vector<itString> dirs = fsysDirListUtil::BuildDirectoryList( anim_loc );
			int numdirs = dirs.size();
			for ( int j = 0 ; j < numdirs && !m_pAnimKeys ; ++j )
			{
				anim_loc.Push( dirs[j] );
				anim_loc.Push( gfPaths::GetSubPath(gfPaths::e_Animation) );
				anim_loc.Push( m_AnimFilename.GetValue() );

				//	debug only
				//std::string tempstr;
				//fsFileUtil::LocatorToANSIFilename(anim_loc, tempstr);
				//DBG_LOG3( "%d-%d - %s", i, j, tempstr.c_str() );

				if ( fsFileUtil::FileExists( anim_loc ) )
				{
					m_pAnimKeys = entImport::LoadAnimKeys( anim_loc );
					if (m_pAnimKeys)
					{
						m_pAnimation = entImport::CreateAnimation( *m_pAnimKeys );

						// Instead of setting the animation's frame rate here, 
						// set our frame rate property from the animation's frame rate
						// which was written when the animation was exported. 
						// If this is coming from SetData(), then the existing
						// frame rate will overwrite it, but if this is a new animation
						// then we will use the desired frame rate automagically.
						m_FrameRate.SetValue( m_pAnimation->GetFrameRate() );

						// Set the values we have already into the new animation
						//m_pAnimation->SetFrameRate( m_FrameRate.GetValue() );
						m_pAnimation->SetLooping( m_bLooping.GetValue());
						m_pAnimation->SetStartFrame( m_StartFrame.GetValue() );
						m_pAnimation->SetEndFrame( m_EndFrame.GetValue() );

						check_driver_name();
					}
				}

				anim_loc.Pop();
				anim_loc.Pop();
				anim_loc.Pop();
			}
			this->MarkDirty();
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetDriverToAnimLengthFlag( bool i_bSetFlag, 
													   prtyProperty::UndoFlags i_Undoable)
{
	m_bDriverToAnimLength.SetValue(i_bSetFlag, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetAnimName( const std::string& i_AnimName, 
										 prtyProperty::UndoFlags i_Undoable)
{
	m_AnimName.SetValue(i_AnimName, i_Undoable);
	check_driver_name();
	this->MarkDirty();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetFrameRate( float i_Val, 
										  prtyProperty::UndoFlags i_Undoable)
{
	m_FrameRate.SetValue(i_Val, i_Undoable);
	this->MarkDirty();
	
	if (m_pAnimation)
	{
		m_pAnimation->SetFrameRate( i_Val );
	}
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the beginning of the clip
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetStartFrame( float i_Val, 
										   prtyProperty::UndoFlags i_Undoable)
{
	m_StartFrame.SetValue(i_Val, i_Undoable);
	this->MarkDirty();
	if (m_pAnimation)
	{
		m_pAnimation->SetStartFrame( i_Val );
	}
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the end of the clip
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetEndFrame( float i_Val, 
										 prtyProperty::UndoFlags i_Undoable)
{
	m_EndFrame.SetValue(i_Val, i_Undoable);
	this->MarkDirty();
	if (m_pAnimation)
	{
		m_pAnimation->SetEndFrame( i_Val );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::SetLooping( bool i_bSetFlag, 
										prtyProperty::UndoFlags i_Undoable)
{
	m_bLooping.SetValue(i_bSetFlag, i_Undoable);
	this->MarkDirty();
	if (m_pAnimation)
	{
		m_pAnimation->SetLooping( i_bSetFlag );
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA chtrDriverAnimationSub::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.8431f, 1.0f, 1.0f );
}


//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void chtrDriverAnimationSub::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );

	//	cast the 2nd driver and check it is the same type
	//
	chtrDriverAnimationSub* pSecondDriver = dynamic_cast<chtrDriverAnimationSub*>(io_pDriverAtEnd);
	DBG_ASSERT0( pSecondDriver != 0, "Cannot split with 2 different type of drivers" );

	// NOTE: this is the code from FULL anim for splitting.  Sub-anims currently
	//	don't have "start frame" and "end frame" functionality.

	this->SetDriverToAnimLengthFlag(false);
	pSecondDriver->SetDriverToAnimLengthFlag(false);

	//	split the custom parts of this driver
	//
	float startframe, endframe;
	startframe	= this->GetStartFrame();
	if (startframe == -1)
	{
		startframe = 0;		// is this always true?
	}
	endframe	= this->GetEndFrame();
	if (endframe == -1)
	{
		endframe = m_pAnimation->GetNumFrames();
	}

	float midframe = startframe + (endframe - startframe) * percent;
	pSecondDriver->SetStartFrame( midframe );
	this->SetEndFrame( midframe );

	//	set the driver times accordingly
	tmlnDriver::PerformSplit(io_pDriverAtEnd, i_fTime);
}


//--------------------------------------------------------------------
// Set name of driver from filename or anim name
//--------------------------------------------------------------------
void chtrDriverAnimationSub::check_driver_name()
{
	if (m_AnimName.GetValue().size() > 0)
		this->SetName(m_AnimName.GetValue().c_str());
	else if (m_AnimFilename.GetValue().GetLength() > 0)
	{
		std::string str = itStringUtil::GetStdString(m_AnimFilename.GetValue());
		this->SetName(str.c_str());
	}
}

//--------------------------------------------------------------------
// return true if blending and if true, return amount blending time
//--------------------------------------------------------------------
bool chtrDriverAnimationSub::get_blend_time(float i_Time, float &o_BlendTime)
{
	switch (this->GetBlendType())
	{
	default:
	case tmlnDriver::e_NoBlending:
		return false;
	case tmlnDriver::e_Overwrite:
		o_BlendTime = 0.0f;
		return true;
	case tmlnDriver::e_BlendTime:
		o_BlendTime = this->GetBlendTime();
		return (this->GetBeginTime() - i_Time <= o_BlendTime);
	case tmlnDriver::e_Previous:
		{
			float begin_time = this->GetBeginTime();
			float prev_time = m_Channel.GetPreviousTime(begin_time);
			o_BlendTime = begin_time - prev_time;
			return (begin_time - i_Time <= o_BlendTime);
		}
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDriverAnimationSub::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
//#ifdef _MANAGED
//	if (SystemChtr::chtrDriverAnimationSubForm::FormInstance != nullptr)
//	{
//		SystemChtr::chtrDriverAnimationSubForm::FormInstance->UpdateForm();
//	}
//#endif

	this->MarkDirty();
}
