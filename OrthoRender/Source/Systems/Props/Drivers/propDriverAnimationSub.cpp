/*****************************************************************************
**	propDriverAnimationSub.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Drivers/propDriverAnimationSub.hpp"

#include "Systems/Props/Drivers/propDriverAnimationSubInfo.hpp"
#include "Systems/Props/Drivers/propDriverAnimationSubParser.hpp"
#include "Systems/Props/Drivers/propDriverAnimationSubForm.h"

#include "Support/mnm/mnmDebugInfo.hpp"

#include "Core/It/itStringUtil.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAnimationSub::propDriverAnimationSub(propChannelAnimationSub &i_Channel)
:	m_Channel(i_Channel), 
	m_bDriverToAnimLength("Driver Length to Anim Length",true),
	m_AnimIndex("Anim Index",0),
	m_AnimName("Anim Name")
{
	this->SetRestoreOriginalValue(true);

	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyPropertyUIInfo(&(m_bDriverToAnimLength), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_AnimName), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_AnimIndex), "category", "description");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<propDriverAnimationSub>(this, &propDriverAnimationSub::PropertyChanged));
	m_AnimName.AddCallback(new prtyCallbackWrapper<propDriverAnimationSub>(this, &propDriverAnimationSub::PropertyChanged));
	m_AnimIndex.AddCallback(new prtyCallbackWrapper<propDriverAnimationSub>(this, &propDriverAnimationSub::PropertyChanged));
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  propDriverAnimationSub::Operate(float i_Time)
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
		m_Channel.AddSubAnimation( m_AnimIndex.GetValue(), this->GetBeginTime() );
	}
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  propDriverAnimationSub::GetDriverInfo() const
{
	propDriverAnimationSubInfo *pInfo = new propDriverAnimationSubInfo(propDriverAnimationSubParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	int anim_index = 0;

	pInfo->m_Info.m_AnimIndex = m_AnimIndex.GetValue();
	pInfo->m_Info.m_AnimName = m_AnimName.GetValue();
	pInfo->m_Info.m_bDriverResizeByAnimLength = m_bDriverToAnimLength.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void propDriverAnimationSub::SetDriverInfo(	const propDriverAnimationSubInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info, i_Undoable );

	m_AnimIndex.SetValue(i_Info.m_Info.m_AnimIndex, i_Undoable );
	m_AnimName.SetValue(i_Info.m_Info.m_AnimName, i_Undoable );
	if (m_AnimName.GetValue().GetLength() > 0)
	{
		this->SetName(itStringUtil::GetStdString(m_AnimName.GetValue()).c_str());

		// eventually set category by name of root of animation
		this->SetCategory(itStringUtil::GetStdString(m_AnimName.GetValue()).c_str());
	}
	m_bDriverToAnimLength.SetValue(i_Info.m_Info.m_bDriverResizeByAnimLength, i_Undoable );

	// set m_Channel.

	//this->Update();
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  propDriverAnimationSub::DoEditProperties()
{
#ifdef _MANAGED
	//StudioFramework::
	SystemProp::propDriverAnimationSubForm ^ form =
		gcnew SystemProp::propDriverAnimationSubForm( *this );

//	form->Show();
	tmaDialogTabbedMgr::RemoveTabPages("Driver");
	tmaDialogTabbedMgr::AddTabPage("Driver", form->GetTabPage(0));

	tmlnDriver::AddBasePropertiesTab();

	tmaDialogTabbedMgr::Show("Driver");

	// The tab page from our custom form is now in the general
	// driver properties dialog, we can delete the custom form.
	delete form;
#endif
}

//--------------------------------------------------------------------
// Update - update the object
//--------------------------------------------------------------------
void propDriverAnimationSub::Update()
{
	//m_Channel.m_pProp->m_pEntity;
}


//====================================================================
// Get/Set data
//====================================================================

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool propDriverAnimationSub::IsDriverToAnimLength()
{
	return m_bDriverToAnimLength.GetValue();
}
void propDriverAnimationSub::SetDriverToAnimLengthFlag( bool i_bSetFlag, 
													   prtyProperty::UndoFlags i_Undoable)
{
	m_bDriverToAnimLength.SetValue(i_bSetFlag, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const itString&	propDriverAnimationSub::GetAnimName()
{
	return m_AnimName.GetValue();
}
void propDriverAnimationSub::SetAnimName(	const itString& i_AnimName, 
											prtyProperty::UndoFlags i_Undoable)
{
	m_AnimName.SetValue(i_AnimName, i_Undoable);
	if (m_AnimName.GetValue().GetLength() > 0)
		this->SetName(itStringUtil::GetStdString(m_AnimName.GetValue()).c_str());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int propDriverAnimationSub::GetAnimIndex()
{
	return m_AnimIndex.GetValue();
}
void propDriverAnimationSub::SetAnimIndex( int i_AnimIndex, 
											prtyProperty::UndoFlags i_Undoable)
{
	m_AnimIndex.SetValue(i_AnimIndex, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// Adapter
//--------------------------------------------------------------------
propChannelAnimationSub& propDriverAnimationSub::Adapter()
{
	return m_Channel;
}

const propChannelAnimationSub& propDriverAnimationSub::GetAdapter() const
{
	return m_Channel;
}

void  propDriverAnimationSub::SetAdapter(const propChannelAnimationSub& i_Channel)
{
	m_Channel = i_Channel;
	this->Update();
}

//--------------------------------------------------------------------
// return true if blending and if true, return amount blending time
//--------------------------------------------------------------------
bool propDriverAnimationSub::get_blend_time(float i_Time, float &o_BlendTime)
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
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA propDriverAnimationSub::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.8431f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void propDriverAnimationSub::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );

	//	cast the 2nd driver and check it is the same type
	//
	propDriverAnimationSub* pSecondDriver = dynamic_cast<propDriverAnimationSub*>(io_pDriverAtEnd);
	DBG_ASSERT0( pSecondDriver != 0, "Cannot split with 2 different type of drivers" );

	this->SetDriverToAnimLengthFlag(false);
	pSecondDriver->SetDriverToAnimLengthFlag(false);

	// NOTE: this is the code from FULL anim for splitting.  Sub-anims currently
	//	don't have "start frame" and "end frame" functionality.
/*
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
*/
	//	set the driver times accordingly
	tmlnDriver::PerformSplit(io_pDriverAtEnd, i_fTime);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void propDriverAnimationSub::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
#ifdef _MANAGED
	if (SystemProp::propDriverAnimationSubForm::FormInstance != nullptr)
	{
		SystemProp::propDriverAnimationSubForm::FormInstance->UpdateForm();
	}
#endif

	this->MarkDirty();
}
