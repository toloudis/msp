/****************************************************************************\
**	pwxControlMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/pwxControlMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "ToolUIWx/pwx/pwxControl.hpp"
#include "ToolUIWx/pwx/pwxControlFactory.hpp"

#include <vector>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace
{
	std::vector<pwxControlFactory*> l_Factories;
	std::vector<pwxControl*> l_Controls;
	pwxIntCallbackMgr* l_pImplementation = NULL;
}

//---------------------------------------------------------------------------
//	Initialize()
//---------------------------------------------------------------------------
void pwxControlMgr::Initialize()
{
	l_pImplementation = new pwxIntCallbackMgr;
	prtyIntCallbackMgr::SetImplementation(l_pImplementation);
}

//---------------------------------------------------------------------------
//	DeInitialize()
//---------------------------------------------------------------------------
void pwxControlMgr::DeInitialize()
{
	envSTLHelpers::DeleteContainer(l_Factories);

	if (l_pImplementation)
	{
		prtyIntCallbackMgr::SetImplementation(NULL);
		delete l_pImplementation;
	}

	// It really is too late to delete these controls.
	// This list should be empty by now.
	DBG_LOG("At CleanUp, number of property controls: " << l_Controls.size());
	//envSTLHelpers::DeleteContainer(l_Controls);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxControlMgr::AddControlFactory(pwxControlFactory* i_pControlFactory)
{
	l_Factories.push_back(i_pControlFactory);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxControlMgr::RemoveControlFactory(pwxControlFactory* i_pControlFactory)
{
	envSTLHelpers::RemoveOneValue(l_Factories, i_pControlFactory);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int pwxControlMgr::GetNumControls()
{
	return l_Controls.size();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//pwxControl* pwxControlMgr::GetControl(const std::string& i_ControlName)
//{
//	return NULL;
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxControl* pwxControlMgr::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 wxWindow* i_pParent)
{	
	DBG_ASSERT(i_pUIInfo != 0, "Cannot create a control with a NULL property UI Info");

	std::vector<pwxControlFactory*>::iterator it;
	for (it = l_Factories.begin(); it != l_Factories.end(); ++it)
	{
		pwxControlFactory *pFactory = (*it);

		pwxControl *pControl = pFactory->CreateControl(i_pUIInfo, i_pParent);
		if (pControl)
		{
			l_Controls.push_back(pControl);
			return pControl;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
// Returns true if the control was found and deleted.
//----------------------------------------------------------------------------
bool pwxControlMgr::DeleteControl(wxWindow* i_pControl)
{
//	return envSTLHelpers::DeleteOneValue(l_Controls, i_pControl);

	std::vector<pwxControl*>::iterator it;
	for (it = l_Controls.begin(); it != l_Controls.end(); ++it)
	{
		pwxControl *pControl = (*it);
		if (pControl->GetControl() == i_pControl)
		{
			// Delete the control here, remove the element from the vector later
			delete pControl;
			break;
		}
	}

	// Remove the item from the vector
	if (it != l_Controls.end())
	{
		l_Controls.erase(it);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
// Update controls for this UIInfo
//----------------------------------------------------------------------------
void pwxControlMgr::UpdateControl( prtyPropertyUIInfo* i_pUII )
{
	std::vector<pwxControl*>::iterator it;
	for (it = l_Controls.begin(); it != l_Controls.end(); ++it)
	{
		pwxControl *pControl = (*it);
		if (pControl->HasUIInfo(i_pUII))
		{
			pControl->UpdateControl(i_pUII);
			// continue on in loop to find multiple controls with same UIInfo
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void pwxIntCallbackMgr::PropertyChanged(prtyProperty* i_pProperty, int i_Index)
{
	// nothing to do here in wxWidgets
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void pwxIntCallbackMgr::UpdateControl( prtyPropertyUIInfo* i_pUII )
{
	pwxControlMgr::UpdateControl( i_pUII );
}

#endif
