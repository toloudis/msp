/*****************************************************************************
**	tmlnDriverSpline.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Drivers/Spline/tmlnDriverSpline.hpp"

#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/spln/splnCurveMgr.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnCurveSelect.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
//#include "Drivers/Spline/tmlnDriverSplineForm.h"	// GUI

#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
//#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"

#include <boost/bind.hpp>

//============================================================================
//============================================================================
const char* c_SplineGroup = "DriverSpline";


//--------------------------------------------------------------------
// The parent object pointer should be the tmlnScriptObject that
//	 this driver will control. It is used to associate the 
//	 driver icons back to the main object.
//--------------------------------------------------------------------
tmlnDriverSpline::tmlnDriverSpline(tmlnChannelPosition &i_ChannelP, 
								   chDefs::Name i_ChunkName, 
								   pick3dPickObject* i_pParent)
:	m_ChannelP(i_ChannelP), 
	m_pSpline(new splnSpline), 
	m_ChunkNameDS(i_ChunkName),
	//m_SelectFirstPoint(""),
	//m_SelectAllPoints(""),
	//m_AppendAfterSelected(""),
	m_AppendAtObjectPosition("Append")
{
	splnCurveMgr::AddCurve(c_SplineGroup, m_pSpline, i_pParent);  // splnCurveMgr takes ownership

	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	// Register property UI Info
	prtyButtonUIInfo* pPBUII;
	//pPBUII = new prtyButtonUIInfo(&m_SelectFirstPoint, "Select", "Select First Point");
	//pPBUII->SetText("Select First Point");
	//AddProperty( pPBUII );
	//pPBUII = new prtyButtonUIInfo(&m_SelectAllPoints, "Select", "Select All Points");
	//pPBUII->SetText("Select All Points");
	//AddProperty( pPBUII );
	//pPBUII = new prtyButtonUIInfo(&m_AppendAfterSelected, "New Point", "Append After Selected");
	//pPBUII->SetText("Append After Selected");
	//AddProperty( pPBUII );
	pPBUII = new prtyButtonUIInfo(&m_AppendAtObjectPosition, "New Point", "Append At Object Position");
	pPBUII->SetText("Append At Object Position");
	AddProperty( pPBUII );

	// Register callbacks to update dirty bit when properties change
	//m_SelectFirstPoint.AddCallback(new prtyCallbackWrapper<tmlnDriverSpline>(this, &tmlnDriverSpline::ButtonClicked));
	//m_SelectAllPoints.AddCallback(new prtyCallbackWrapper<tmlnDriverSpline>(this, &tmlnDriverSpline::ButtonClicked));
	//m_AppendAfterSelected.AddCallback(new prtyCallbackWrapper<tmlnDriverSpline>(this, &tmlnDriverSpline::ButtonClicked));
	m_AppendAtObjectPosition.AddCallback(new prtyCallbackWrapper<tmlnDriverSpline>(this, &tmlnDriverSpline::AppendAtObjectPositionClicked));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSpline::~tmlnDriverSpline()
{
	splnCurveMgr::DeleteCurve(m_pSpline);
	//delete m_pSpline;	// splnCurveMgr takes ownership
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverSpline::GetDriverInfo() const
{
	tmlnDriverSplineInfo *pInfo = new tmlnDriverSplineInfo(m_ChunkNameDS);

	this->GetBaseDriverInfo(*pInfo);

	int num_pts = m_pSpline->GetNumPoints();
	pInfo->m_SplineInfo.m_Points.resize(num_pts);
	for (int i=0; i<num_pts; i++)
		pInfo->m_SplineInfo.m_Points[i] = m_pSpline->GetPointPos(i);

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverSpline::SetDriverInfo(const tmlnDriverSplineInfo& i_Info, prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	m_pSpline->SetPoints(&i_Info.m_SplineInfo.m_Points[0],
		i_Info.m_SplineInfo.m_Points.size());

	this->Update();
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverSpline::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maPoint3d goal = m_pSpline->Evaluate(0.0f);
		maPoint3d cur = m_ChannelP.GetPosition();
		float percent = this->GetBlendAlpha(m_ChannelP.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal*percent + cur*(1.0f - percent);	

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			// Get tangent of spline at start, use it in blending
			maVector3d begin_tangent(0,0,0);
			maPoint3d pt1 = m_pSpline->Evaluate(0.0f, &begin_tangent);

			// The tangent from Evaluate is not enough, we need to get a sense
			// of the speed of the movement along the spline and use
			// that as the length of the gradient.
			const float small_delta = 1.0f / 25.0f;
			if (this->GetDuration() > small_delta)
			{
				// Go ahead in time 1/25 of a second and evaluate there
				float percent = small_delta / this->GetDuration();
				maPoint3d pt2 = m_pSpline->Evaluate(percent);
				float magnitude = (pt2-pt1).Length() / small_delta;
				begin_tangent *= magnitude;
			}

			AddGradientInfluence(&m_ChannelP, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, pos, &begin_tangent);
		}

		m_ChannelP.SetPosition(pos);
	}
	else
	{
		// within driver range
		float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		maPoint3d pos = m_pSpline->Evaluate(percent);
		m_ChannelP.SetPosition(pos);
	}
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
//void  tmlnDriverSpline::DoEditProperties()
//{
//#ifdef _MANAGED
//	//System::Windows::Forms::MessageBox::Show("Spline Properties!");
//	StudioFramework::tmlnDriverSplineForm ^form =
//		gcnew StudioFramework::tmlnDriverSplineForm(*this);
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
// Spline
//--------------------------------------------------------------------
splnSpline& tmlnDriverSpline::Spline()
{
	return *m_pSpline;
}
const splnSpline & tmlnDriverSpline::GetSpline() const
{
	return *m_pSpline;
}
void  tmlnDriverSpline::SetSpline(const splnSpline &i_Spline)
{
	(*m_pSpline) = i_Spline;
	this->Update();

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Update - if you are going to alter the spline directly,
//	call this to update its representation
//--------------------------------------------------------------------
void tmlnDriverSpline::Update()
{
	splnCurveMgr::UpdateCurve(m_pSpline);
}

//--------------------------------------------------------------------
//  Driver should select its 3D icon
//--------------------------------------------------------------------
//virtual 
void  tmlnDriverSpline::DoSelectIcon()
{
	splnCurvePointObject *pObj = splnCurveSelect::GetControlPointObject(m_pSpline, 0);
	if (pObj)
	{
		sel3dMgr::CreateUndoOperation();
		sel3dMgr::Select(pObj);
	}
}

//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void tmlnDriverSpline::ShowIcons( bool i_bVisible )
{
	splnCurveMgr::SetRenderable(m_pSpline, i_bVisible);
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverSpline::GetClipFillColor() const
{
	return maFloatRGBA( 0.9843f, 0.7725f, 0.5569f, 1.0f );
}

//--------------------------------------------------------------------
// Return the current position of the channel, used to place
//	new control points in position of object on channel.
//--------------------------------------------------------------------
maPoint3d tmlnDriverSpline::GetChannelCurrentPosition() const
{
	return m_ChannelP.GetPosition();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverSpline::GetBeginValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	if (&m_ChannelP == i_pChannel)
	{
		o_Value = m_pSpline->Evaluate(0.0f);
	}
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverSpline::GetEndValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	if (&m_ChannelP == i_pChannel)
	{
		o_Value = m_pSpline->Evaluate(1.0f);
	}
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverSpline::GetEndGradient(tmlnChannel* i_pChannel, maVector3d& o_Gradient)
{
	if (&m_ChannelP == i_pChannel)
	{
		// The tangent from Evaluate is not enough, we need to get a sense
		// of the speed of the movement along the spline and use
		// that as the length of the gradient.
		maVector3d gradient(0,0,0);
		maPoint3d pt2 = m_pSpline->Evaluate(1.0f, &gradient);

		const float small_delta = 1.0f / 25.0f;
		if (this->GetDuration() > small_delta)
		{
			// Go back in time 1/25 of a second and evaluate there
			float percent = (this->GetDuration() - small_delta) / this->GetDuration();
			maPoint3d pt1 = m_pSpline->Evaluate(percent);
			float magnitude = (pt2-pt1).Length() / small_delta;
			gradient *= magnitude;
		}
		o_Gradient = gradient;
	}
}

////--------------------------------------------------------------------
//// Select first point in spline
////--------------------------------------------------------------------
//void tmlnDriverSpline::select_first_point()
//{
//	sel3dMgr::CreateUndoOperation();
//	sel3dMgr::Select(splnCurveSelect::GetControlPointObject(m_pSpline, 0));
//}
//
////--------------------------------------------------------------------
//// Select all points in spline
////--------------------------------------------------------------------
//void tmlnDriverSpline::select_all_points()
//{
//	int num_points = m_pSpline->GetNumPoints();
//	if (num_points > 0)
//	{
//		sel3dMgr::CreateUndoOperation();
//		sel3dMgr::Select(splnCurveSelect::GetControlPointObject(m_pSpline, 0));
//		for (int i=1; i<num_points; ++i)
//			sel3dMgr::AddToSelection(splnCurveSelect::GetControlPointObject(m_pSpline, i));
//	}
//}
//
////--------------------------------------------------------------------
//// Append new control point after selected control point
////--------------------------------------------------------------------
//void tmlnDriverSpline::append_after_selected()
//{					
//	splnCurvePointObject* pPoint = dynamic_cast<splnCurvePointObject*>(sel3dMgr::GetSelected());
//	if (pPoint && (pPoint->GetCurve() == m_pSpline))
//	{
//		splnCurveMgr::InsertCtrlPoint(m_pSpline, pPoint->GetPointIndex());
//	}
//	else
//	{
//		splnCurveMgr::InsertCtrlPoint(m_pSpline, -1);	// -1 means insert at end
//	}
//}

//--------------------------------------------------------------------
// Append new control point at current channel's value
//--------------------------------------------------------------------
void tmlnDriverSpline::append_at_channel_value()
{
	splnCurveMgr::AppendCtrlPoint(m_pSpline, this->GetChannelCurrentPosition());
}

//--------------------------------------------------------------------
// button click callbacks
//--------------------------------------------------------------------
//void tmlnDriverSpline::ButtonClicked(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	// It is not safe to alter the selection within a property callback.
//	// Selection changes cause the interface to alter such that the control
//	// that triggered this property might be destroyed.
//	// Use the mnmThinkMgr to execute the actual operation later.
//	if (i_pProperty == &m_SelectFirstPoint)
//	{
//		mnmThinkMgr::CallFunctionDelayed( boost::bind(
//							&tmlnDriverSpline::select_first_point, this) );
//	}
//	else if (i_pProperty == &m_SelectAllPoints)
//	{
//		mnmThinkMgr::CallFunctionDelayed( boost::bind(
//							&tmlnDriverSpline::select_all_points, this) );
//	}
//	else if (i_pProperty == &m_AppendAfterSelected)
//	{
//		mnmThinkMgr::CallFunctionDelayed( boost::bind(
//							&tmlnDriverSpline::append_after_selected, this) );
//	}
//	else if (i_pProperty == &m_AppendAtObjectPosition)
//	{
//		mnmThinkMgr::CallFunctionDelayed( boost::bind(
//							&tmlnDriverSpline::append_at_channel_value, this) );
//	}
//}
void tmlnDriverSpline::AppendAtObjectPositionClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
		mnmThinkMgr::CallFunctionDelayed( boost::bind(
						&tmlnDriverSpline::append_at_channel_value, this) );
}