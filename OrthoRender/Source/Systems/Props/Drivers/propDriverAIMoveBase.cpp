/*****************************************************************************
**	propDriverAIMoveBase.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "propDriverAIMoveBase.hpp"

#include "propDriverAIMoveBaseInfo.hpp"
#include "propDriverAIMoveBaseParser.hpp"
#include "propDriverAIMoveBaseForm.h"
#include "propDriverAnimationFullForm.h"
#include "propScriptObject.hpp"

#include "mnmDebugInfo.hpp"
#include "tmlnChannelPosition.hpp"
#include "tmlnDriverSplineForm.h"

#include "itStringUtil.hpp"
#include "tmaDialogTabbedMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAIMoveBase::propDriverAIMoveBase( propScriptObject* i_pObject )
{
	m_pDriverAnimationFull = new propDriverAnimationFull( i_pObject->ChannelAnimationFull() );
	m_pDriverAnimationFull->SetInitialTime();

	m_pDriverSpline	= new tmlnDriverSplineOriented( i_pObject->ChannelPosition(), i_pObject->ChannelOrientation(), propDriverAIMoveBaseParser::GetSplineChunkName(), i_pObject );
	m_pDriverSpline->SetInitialTime();

	// spline
	splnSpline spline;
	spline.AppendPoint( i_pObject->ChannelPosition().GetPosition() );
	spline.AppendPoint( i_pObject->ChannelPosition().GetPosition() + maPoint3d( 0.0f, 2.5f, 0.0f ) );
	m_pDriverSpline->SetSpline( spline );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAIMoveBase::~propDriverAIMoveBase()
{
	delete m_pDriverAnimationFull;
	delete m_pDriverSpline;
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  propDriverAIMoveBase::Operate(float i_Time)
{
	m_pDriverSpline->Operate( i_Time );
	m_pDriverAnimationFull->Operate( i_Time );
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  propDriverAIMoveBase::GetDriverInfo() const
{
	propDriverAIMoveBaseInfo *pInfo = new propDriverAIMoveBaseInfo(propDriverAIMoveBaseParser::GetChunkName());

	delete pInfo->m_pSplineInfo;
	(pInfo->m_pSplineInfo) = dynamic_cast<tmlnDriverSplineInfo*>(m_pDriverSpline->GetDriverInfo());
	delete pInfo->m_pAnimFullInfo;
	(pInfo->m_pAnimFullInfo) = dynamic_cast<propDriverAnimationFullInfo*>(m_pDriverAnimationFull->GetDriverInfo());

	//
	pInfo->m_AIMoveBaseInfo.m_Stub = 0;

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void propDriverAIMoveBase::SetDriverInfo(const propDriverAIMoveBaseInfo& i_Info, prtyProperty::UndoFlags i_Undoable)
{
	m_pDriverAnimationFull->SetDriverInfo( *(i_Info.m_pAnimFullInfo), i_Undoable );
	m_pDriverSpline->SetDriverInfo( *(i_Info.m_pSplineInfo), i_Undoable );

	//this->xxx = i_Info.m_AIMoveBaseInfo.m_Stub;
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  propDriverAIMoveBase::DoEditProperties()
{
	SystemProp::propDriverAIMoveBaseForm ^ form		= gcnew SystemProp::propDriverAIMoveBaseForm( *this );
	SystemProp::propDriverAnimationFullForm ^ formAF = gcnew SystemProp::propDriverAnimationFullForm( *m_pDriverAnimationFull );
	StudioFramework::tmlnDriverSplineForm ^ formSP	= gcnew StudioFramework::tmlnDriverSplineForm( *m_pDriverSpline );

//	form->AddTabPage( formAF->GetTabPage(0) );
//	form->AddTabPage( formSP->GetTabPage(0) );
//	form->Show();
	tmaDialogTabbedMgr::RemoveTabPages("Driver");
	tmaDialogTabbedMgr::AddTabPage("Driver", form->GetTabPage(0));
	tmaDialogTabbedMgr::AddTabPage("Driver", formAF->GetTabPage(0));
	tmaDialogTabbedMgr::AddTabPage("Driver", formSP->GetTabPage(0));

	tmlnDriver::AddBasePropertiesTab();

	tmaDialogTabbedMgr::Show("Driver");
}

//--------------------------------------------------------------------
// Update - update the object
//--------------------------------------------------------------------
void propDriverAIMoveBase::Update()
{
	//m_ChannelP.m_pProp->m_pEntity;
}


//====================================================================
// Get/Set data
//====================================================================

//--------------------------------------------------------------------
// Adapters
//--------------------------------------------------------------------
//propChannelPosition& propDriverAIMoveBase::AdapterPosition()
//{
//	return m_ChannelP;
//}
//const propChannelPosition& propDriverAIMoveBase::GetAdapterPosition() const
//{
//	return m_ChannelP;
//}
//void  propDriverAIMoveBase::SetAdapterPosition(const propChannelPosition& i_ChannelP)
//{
//	m_ChannelP = i_ChannelP;
//	this->Update();
//}
//
//propChannelOrientation& propDriverAIMoveBase::AdapterOrientation()
//{
//	return m_ChannelO;
//}
//const propChannelOrientation& propDriverAIMoveBase::GetAdapterOrientation() const
//{
//	return m_ChannelO;
//}
//void  propDriverAIMoveBase::SetAdapterOrientation(const propChannelOrientation& i_ChannelO)
//{
//	m_ChannelO = i_ChannelO;
//	this->Update();
//}
//
//propChannelAnimationFull& propDriverAIMoveBase::AdapterAnimationFull()
//{
//	return m_ChannelAF;
//}
//const propChannelAnimationFull& propDriverAIMoveBase::GetAdapterAnimationFull() const
//{
//	return m_ChannelAF;
//}
//void  propDriverAIMoveBase::SetAdapterAnimationFull(const propChannelAnimationFull& i_ChannelAF)
//{
//	m_ChannelAF = i_ChannelAF;
//	this->Update();
//}

//--------------------------------------------------------------------
// return true if blending and if true, return amount blending time
//--------------------------------------------------------------------
bool propDriverAIMoveBase::get_blend_time(float i_Time, float &o_BlendTime)
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
	// FIX: - get this working.
	//		float prev_time = m_ChannelP.GetPreviousTime(begin_time);
	//		o_BlendTime = begin_time - prev_time;
	//		return (begin_time - i_Time <= o_BlendTime);
		}
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool propDriverAIMoveBase::IsDriverToAnimLength()
{
	return this->m_pDriverAnimationFull->IsDriverToAnimLength();
}
void propDriverAIMoveBase::SetDriverToAnimLengthFlag( bool i_bSetFlag )
{
	this->m_pDriverAnimationFull->SetDriverToAnimLengthFlag( i_bSetFlag );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string& propDriverAIMoveBase::GetAnimName() const
{
	return this->m_pDriverAnimationFull->GetAnimName();
}
void propDriverAIMoveBase::SetAnimName( const std::string& i_AnimName )
{
	this->m_pDriverAnimationFull->SetAnimName(i_AnimName);
	if (i_AnimName.size() > 0)
		this->m_pDriverAnimationFull->SetName(i_AnimName.c_str());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//int propDriverAIMoveBase::GetAnimIndex()
//{
//	return this->m_pDriverAnimationFull->GetAnimIndex();
//}
//void propDriverAIMoveBase::SetAnimIndex( int i_AnimIndex )
//{
//	this->m_pDriverAnimationFull->SetAnimIndex( i_AnimIndex );
//}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA propDriverAIMoveBase::GetClipFillColor() const
{
	return maFloatRGBA( 0.333f, 0.900f, 0.333f, 1.0f );
}
