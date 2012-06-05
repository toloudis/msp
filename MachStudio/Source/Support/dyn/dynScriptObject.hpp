/*****************************************************************************
**	dynScriptObject.hpp
**
**	This class adds dynamic channels to the base ScriptObject
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DYN_SCRIPTOBJECT_HPP
#error dynScriptObject.hpp multiply included
#endif
#define DYN_SCRIPTOBJECT_HPP

#ifndef DYN_CHANNELTRANSFORMCONTROL_HPP
#include "Support/dyn/dynChannelTransformControl.hpp"
#endif
#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif
#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 


#include <vector>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class tmlnChannel;
class dynPropertyObject;
class sel3dObject;


//============================================================================
//============================================================================
class dynScriptObject : public cmmScriptObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes object to attach control animations to
	//--------------------------------------------------------------------
	dynScriptObject(api3dObject* i_pObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dynScriptObject();

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable 
	// property objects
	//--------------------------------------------------------------------
	void SetParent(relObject &i_Parent);

	//--------------------------------------------------------------------
	//	Adds control to object according to given data
	//--------------------------------------------------------------------
	void AddControl(const dynControlData &i_Data);

	//--------------------------------------------------------------------
	//	Alter control's data
	//--------------------------------------------------------------------
	//void AlterControl(int i_Index, const dynControlData &i_Data);

	//--------------------------------------------------------------------
	// Delete control with given index.
	// If i_bDeleteControlDrivers is true, delete the drivers
	// attached to the control also.
	//--------------------------------------------------------------------
	void DeleteControl(int i_Index, bool i_bDeleteControlDrivers);
	void DeleteControl(const std::string& i_ControlName, bool i_bDeleteControlDrivers);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumControls() const;

	//--------------------------------------------------------------------
	// Access to channel
	//--------------------------------------------------------------------
	const tmlnChannel* GetControl(int i_Index) const;
	tmlnChannel* GetControl(int i_Index);

	//--------------------------------------------------------------------
	// Get type of control at given index
	//--------------------------------------------------------------------
	//dynControlData::ControlType GetControlType(int i_Index) const;

	//--------------------------------------------------------------------
	// Get control data with CURRENT values, for use with GUI sliders.
	//--------------------------------------------------------------------
	const dynControlData& GetControlData(int i_Index) const;

	//--------------------------------------------------------------------
	// Get vector of controls in order to store info to file
	//--------------------------------------------------------------------
	void GetControlData(std::vector<dynControlData> &o_Data) const;

	//--------------------------------------------------------------------
	// Create controls to match data list
	//--------------------------------------------------------------------
	void SetControlData(const std::vector<dynControlData> &i_Controls);

	//------------------------------------------------------------------------
	// Access to name of contorl for display
	//------------------------------------------------------------------------
	const std::string&  GetControlName(int i_Index) const;

	//--------------------------------------------------------------------
	// Return index for control with given name. Returns -1
	//	if not found.
	//--------------------------------------------------------------------
	int GetIndexForName(const std::string &i_Name) const;

	//--------------------------------------------------------------------
	//	Return Control ui properties
	//--------------------------------------------------------------------
	dynPropertyObject* GetControlUI(int i_Index) const;
	dynPropertyObject* GetControlUI(const std::string& i_ControlName) const;

	////--------------------------------------------------------------------
	//// Pass angle to channel control with given index
	////--------------------------------------------------------------------
	//void SetAngle(int i_Index, float i_Angle);

	////--------------------------------------------------------------------
	//// Pass angle to channel control with given index
	////--------------------------------------------------------------------
	//void SetAngle(int i_Index, 
	//	         dynControlData::Axis i_Axis, 
	//			 float i_Angle,
	//			 bool i_bFromDriver = false);

	////--------------------------------------------------------------------
	//// Pass vector to channel control with given index
	////--------------------------------------------------------------------
	//void SetTranslation(int i_Index, 
	//					const maVector3d &i_Trans,
	//					bool i_bFromDriver = false);

	////--------------------------------------------------------------------
	//// Pass vector to channel control with given index
	////--------------------------------------------------------------------
	//void SetScale(int i_Index, 
	//			  const maVector3d &i_Scale);

	//--------------------------------------------------------------------
	//  Get list of possible reference attachment points.
	//--------------------------------------------------------------------
	void  GetReferenceList(std::vector<std::string> &o_List) const;

	//--------------------------------------------------------------------
	//	Store selected index in order to maintain it when re-selecting
	//--------------------------------------------------------------------
	void SetLastSelectedControlIndex(int i_Index);
	int GetLastSelectedControlIndex() const;

private:
	api3dObject* m_pObject;
	shared_ptr<relRelationship> m_ParentRelationship;
	std::vector<dynPropertyObject*> m_PropertyObjects;
	std::vector<tmlnChannel*> m_Controls;
	int m_LastSelectedControlIndex;

};
