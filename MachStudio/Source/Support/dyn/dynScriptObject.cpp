/*****************************************************************************
**	dynScriptObject.cpp
**
**	This class adds dynamic channels to the base ScriptObject
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/dynScriptObject.hpp"

#include "Support/dyn/dynChannelControl.hpp"
#include "Support/dyn/GUI/dynDialogUtil.hpp"
#include "Support/dyn/GUI/dynPropertyObject.hpp"
#include "Support/tmln/tmlnDriver.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/sc/scRotateControl.hpp"
#include "Graphics/sc/scTransformControl.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"


namespace
{
	//--------------------------------------------------------------------
	// Get scObject from api3dObject. 
	//--------------------------------------------------------------------
	scObject* get_object(api3dObject* i_pObject)
	{
		api3dObjectSingle * pGeom = dynamic_cast<api3dObjectSingle *>(i_pObject);
		if (pGeom)
			return pGeom->Object();
		return NULL;
	}

	//--------------------------------------------------------------------
	//	Get control and node from channel
	//--------------------------------------------------------------------
	void get_control_from_channel(tmlnChannel* i_pChannel,
								  scControlAnim* &o_pControl,
								  g3dSceneNode* &o_pNode)
	{
		o_pControl = NULL;
		o_pNode = NULL;

		dynChannelControl *transform_channel = dynamic_cast<dynChannelControl *>(i_pChannel);
		if (transform_channel)
		{
			o_pControl = transform_channel->GetControlAnimation();
			o_pNode = transform_channel->GetControlAnimation()->GetSceneNode();
		}
	}

}	// end of namespace

//--------------------------------------------------------------------
// Constructor takes object to attach control animations to
//--------------------------------------------------------------------
dynScriptObject::dynScriptObject(api3dObject* i_pObject)
: m_pObject(i_pObject),
  m_LastSelectedControlIndex(-1)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynScriptObject::~dynScriptObject()
{
	// don't delete channels, they are owned by tmlnScriptObject base class
//	envSTLHelpers::DeleteContainer(m_Controls);
	envSTLHelpers::DeleteContainer(m_PropertyObjects);
}


//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable 
// property objects
//--------------------------------------------------------------------
void dynScriptObject::SetParent(relObject &i_Parent)
{
	// Create new multiple relationship connecting the property
	// objects to the new parent object
	//
	m_ParentRelationship.reset(
		new relRelationshipMultiple<dynPropertyObject>("Controls", i_Parent, m_PropertyObjects));
	i_Parent.AddRelationship(m_ParentRelationship);

	// update fragments if already "gathered".
	for (int i = 0; i < m_PropertyObjects.size(); i++)
	{
		m_PropertyObjects[i]->SetParentRelationship(m_ParentRelationship);
	}
}


//--------------------------------------------------------------------
//	Adds control to object according to given data
//--------------------------------------------------------------------
void dynScriptObject::AddControl(const dynControlData &i_Data)
{
	// get scObject from api3dObject
	scObject* pObject = get_object(m_pObject);
	if (pObject)
	{
		// get named g3dSceneNode as attachment node
		g3dSceneNode *node = pObject->InsertControlNode(i_Data.m_Node.GetValue().c_str());
		if (node)
		{
			std::string node_name = i_Data.m_Node.GetValue() + "_control";
			node->SetName(node_name.c_str());

			scTransformControl *control = new scTransformControl(node);
			//control->SetScale(i_Data.m_Scale);
			pObject->AddControlAnimation(control);

			dynPropertyObject *pPrtyObject = new dynPropertyObject(i_Data, control);
			m_PropertyObjects.push_back(pPrtyObject);
			pPrtyObject->SetParentRelationship(m_ParentRelationship);

			dynChannelControl *channel = new dynChannelControl(i_Data.m_Name.GetValue().c_str(), 
															   pPrtyObject->GetPropertyRotateX(),
															   pPrtyObject->GetPropertyRotateY(),
															   pPrtyObject->GetPropertyRotateZ(),
															   pPrtyObject->GetPropertyTranslation(),
															   pPrtyObject->GetPropertyScale(),
															   pPrtyObject->GetControlProxy());
			this->AddChannel(channel);
			m_Controls.push_back(channel);
		}
	}

}

//--------------------------------------------------------------------
//	Alter control's data
//--------------------------------------------------------------------
//void dynScriptObject::AlterControl(int i_Index, const dynControlData &i_Data)
//{
//	DBG_ASSERT(i_Index < m_Controls.size(), "Index %d out of range", i_Index);
//
//	// see if attachment node or control type
//	// has changed and remake the controlnode
//	if ((i_Data.m_Node != m_Data[i_Index].m_Node) ||
//		(i_Data.m_ControlType != m_Data[i_Index].m_ControlType))
//	{
//		this->DeleteControl(i_Index, true);
//		this->AddControl(i_Data);
//	}
//	else if (i_Data.m_ControlType == dynControlData::e_RotateControl)
//	{
//		// Alter data in existing control
//		dynChannelRotateControl *channel = dynamic_cast<dynChannelRotateControl *>(m_Controls[i_Index]);
//		channel->SetAxis(dynChannelRotateControl::Axis(i_Data.m_Axis));
//		channel->SetName(i_Data.m_Name.c_str());
//		channel->SetOriginalValue(i_Data.m_Value);
//
//		m_Data[i_Index] = i_Data;
//	}
//	else 
//	{
//		// Alter data in existing control
//		dynChannelControl *channel = dynamic_cast<dynChannelControl *>(m_Controls[i_Index]);
//		channel->SetName(i_Data.m_Name.c_str());
//		channel->SetOriginalRotation(i_Data.m_Rotation);
//		channel->SetOriginalTranslation(i_Data.m_Translation);
//		channel->GetControlAnimation()->SetScale(i_Data.m_Scale);
//				
//		m_Data[i_Index] = i_Data;
//	}
//}

//--------------------------------------------------------------------
// Delete control with given index
//--------------------------------------------------------------------
void dynScriptObject::DeleteControl(const std::string& i_ControlName, 
									bool i_bDeleteControlDrivers)
{
	int index = this->GetIndexForName(i_ControlName);
	if (index >= 0)
		DeleteControl(index, i_bDeleteControlDrivers);
}
void dynScriptObject::DeleteControl(int i_Index, 
									bool i_bDeleteControlDrivers)
{
	DBG_ASSERT(i_Index < m_Controls.size(), "Index out of range - " << i_Index);

	scObject* pObject = get_object(m_pObject);
	scControlAnim *control = NULL;
	g3dSceneNode *node = NULL;
	tmlnChannel *channel = m_Controls[i_Index];

	get_control_from_channel(channel, control, node);
	DBG_ASSERT(control, "Couldn't get control animation to delete");

	pObject->RemoveControlAnimation(control);
	pObject->RemoveControlNode(node);
	delete control;
	delete node;

	if (i_bDeleteControlDrivers)
	{
		// Get all drivers from channel, and delete them since they are
		// attached to a channel that will not exist anymore
		const int num_drivers = channel->GetNumDrivers();
		std::vector<tmlnDriver*> drivers(num_drivers);
		for (int i=0; i<num_drivers; i++)
		{
			// gather drivers
			drivers[i] = &(channel->Driver(i));
		}
		for (int i=0; i<num_drivers; i++)
		{
			// remove drivers
			this->RemoveDriver(drivers[i]);
		}
		// delete drivers
		envSTLHelpers::DeleteContainer(drivers);
	}

	this->RemoveChannel(channel);
	delete channel;
	m_Controls.erase(m_Controls.begin() + i_Index);

	delete m_PropertyObjects[i_Index];
	m_PropertyObjects.erase(m_PropertyObjects.begin() + i_Index);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int dynScriptObject::GetNumControls() const
{
	return m_PropertyObjects.size();
}

//--------------------------------------------------------------------
// Access to channel
//--------------------------------------------------------------------
const tmlnChannel* dynScriptObject::GetControl(int i_Index) const
{
	DBG_ASSERT(i_Index < m_Controls.size(), "Index out of range - " << i_Index);
	return m_Controls[i_Index];
}
tmlnChannel* dynScriptObject::GetControl(int i_Index)
{
	DBG_ASSERT(i_Index < m_Controls.size(), "Index out of range - " << i_Index);
	return m_Controls[i_Index];
}



//--------------------------------------------------------------------
// Get type of control at given index
//--------------------------------------------------------------------
//dynControlData::ControlType dynScriptObject::GetControlType(int i_Index) const
//{
//	return m_Data[i_Index].m_ControlType;
//}

//--------------------------------------------------------------------
// Get control data with CURRENT values, for use with GUI sliders.
//--------------------------------------------------------------------
const dynControlData& dynScriptObject::GetControlData(int i_Index) const
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Index out of range - " << i_Index);
	return m_PropertyObjects[i_Index]->GetControlData();
}

//--------------------------------------------------------------------
// Get vector of controls in order to store info to file
//--------------------------------------------------------------------
void dynScriptObject::GetControlData(std::vector<dynControlData> &o_Data) const
{
	int num_ctrls = m_PropertyObjects.size();
	o_Data.resize(num_ctrls);
	for (int i=0; i<num_ctrls; i++)
		o_Data[i] = m_PropertyObjects[i]->GetControlData();
}


//--------------------------------------------------------------------
// Create controls to match data list
//--------------------------------------------------------------------
void dynScriptObject::SetControlData(const std::vector<dynControlData> &i_Controls)
{
	// remove old controls
	int num_ctrls = m_PropertyObjects.size();
	for (int i=num_ctrls-1; i>=0; i--)
		this->DeleteControl(i, false);

	num_ctrls = i_Controls.size();
	for (int i=0; i<num_ctrls; i++)
		this->AddControl(i_Controls[i]);
}

//------------------------------------------------------------------------
// Access to name of contorl for display
//------------------------------------------------------------------------
const std::string&  dynScriptObject::GetControlName(int i_Index) const
{
	DBG_ASSERT( (i_Index < m_PropertyObjects.size()), "index out of range " << i_Index << " < " <<  (int)m_PropertyObjects.size() );
	return m_PropertyObjects[i_Index]->GetName();
}

//--------------------------------------------------------------------
// Return index for control with given name. Returns -1
//	if nto found.
//--------------------------------------------------------------------
int dynScriptObject::GetIndexForName(const std::string &i_Name) const
{
	for (int i=0; i<m_Controls.size(); i++)
	{
		if (i_Name == m_PropertyObjects[i]->GetName()) // handles generated display name also
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
//	Return Control ui properties
//--------------------------------------------------------------------
dynPropertyObject* dynScriptObject::GetControlUI(int i_Index) const
{	
	DBG_ASSERT( (i_Index < m_PropertyObjects.size()), "index out of range " << i_Index << " < " <<  (int)m_PropertyObjects.size() );
	return m_PropertyObjects[i_Index];
}
dynPropertyObject* dynScriptObject::GetControlUI(const std::string& i_ControlName) const
{
	int index = GetIndexForName(i_ControlName);
	if (index >= 0)
	{
		return m_PropertyObjects[index];
	}

	return NULL;
}

////--------------------------------------------------------------------
//// Pass angle to channel control with given index
////--------------------------------------------------------------------
//void dynScriptObject::SetAngle(int i_Index, float i_Angle)
//{
//	DBG_ASSERT(i_Index < m_Controls.size(), "Index %d out of range", i_Index);
//	dynChannelRotateControl *channel = dynamic_cast<dynChannelRotateControl *>(m_Controls[i_Index]);
//	DBG_ASSERT(channel, "Wrong type of control animation");
//	channel->SetOriginalValue(i_Angle);
//	//channel->SetValue(i_Angle);
//}
//
////--------------------------------------------------------------------
//// Pass angle to channel control with given index
////--------------------------------------------------------------------
//void dynScriptObject::SetAngle(int i_Index, 
//							   dynControlData::Axis i_Axis, 
//							   float i_Angle,
//							   bool i_bFromDriver)
//{
//	DBG_ASSERT(i_Index < m_Controls.size(), "Index %d out of range", i_Index);
//	dynChannelControl *channel = dynamic_cast<dynChannelControl *>(m_Controls[i_Index]);
//	DBG_ASSERT(channel, "Wrong type of control animation");
//
//	maVector3d rot = channel->GetEulerAngles();
//	switch (i_Axis)
//	{
//	default:
//	case dynControlData::e_X:
//		rot.m_X = i_Angle;
//		break;
//	case dynControlData::e_Y:
//		rot.m_Y = i_Angle;
//		break;
//	case dynControlData::e_Z:
//		rot.m_Z = i_Angle;
//		break;
//	}
//
//	// Set new value into control and channel directly 
//	channel->SetEulerAngles(rot, i_bFromDriver);
//
//	// If there are no drivers on this channel, then set the "original"
//	// value also so that it gets saved to file
//	if (channel->GetNumDrivers() == 0)
//	{
//		channel->SetOriginalRotation(rot);
//	}
//}
//
////--------------------------------------------------------------------
//// Pass vector to channel control with given index
////--------------------------------------------------------------------
//void dynScriptObject::SetTranslation(int i_Index, 
//									 const maVector3d &i_Trans,
//									 bool i_bFromDriver)
//{
//	DBG_ASSERT(i_Index < m_Controls.size(), "Index %d out of range", i_Index);
//	dynChannelControl *channel = dynamic_cast<dynChannelControl *>(m_Controls[i_Index]);
//	DBG_ASSERT(channel, "Wrong type of control animation");
//
//	// Set new value into control and channel directly 
//	channel->SetTranslation(i_Trans, i_bFromDriver);
//
//	// If there are no drivers on this channel, then set the "original"
//	// value also so that it gets saved to file
//	if (channel->GetNumDrivers() == 0)
//	{
//		channel->SetOriginalTranslation(i_Trans);
//	}
//}
//
////--------------------------------------------------------------------
//// Pass vector to channel control with given index
////--------------------------------------------------------------------
//void dynScriptObject::SetScale(int i_Index, 
//							   const maVector3d &i_Scale)
//{
//	DBG_ASSERT(i_Index < m_Controls.size(), "Index %d out of range", i_Index);
//	dynChannelControl *channel = dynamic_cast<dynChannelControl *>(m_Controls[i_Index]);
//	DBG_ASSERT(channel, "Wrong type of control animation");
//
//	// Set new value into control directly 
//	channel->GetControlAnimation()->SetScale(i_Scale);
//	m_Data[i_Index].m_Scale = i_Scale;
//}

//--------------------------------------------------------------------
//  Get list of possible reference attachment points.
//--------------------------------------------------------------------
void  dynScriptObject::GetReferenceList(std::vector<std::string> &o_List) const
{
	if (m_pObject)
	{
		m_pObject->GetReferenceList(o_List);
	}
}

//--------------------------------------------------------------------
//	Store selected index in order to maintain it when re-selecting
//--------------------------------------------------------------------
void dynScriptObject::SetLastSelectedControlIndex(int i_Index)
{
	m_LastSelectedControlIndex = i_Index;
}
int dynScriptObject::GetLastSelectedControlIndex() const
{
	return m_LastSelectedControlIndex;
}
