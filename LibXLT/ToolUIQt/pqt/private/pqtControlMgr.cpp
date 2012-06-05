/****************************************************************************\
**	pqtControlMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/pqtControlMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "ToolUIQt/pqt/pqtControl.hpp"
#include "ToolUIQt/pqt/pqtControlFactory.hpp"

#include <vector>


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	std::vector<pqtControlFactory*> l_Factories;
	std::vector<pqtControl*> l_Controls;
	pqtIntCallbackMgr* l_pImplementation = NULL;
}

//---------------------------------------------------------------------------
//	Initialize()
//---------------------------------------------------------------------------
void pqtControlMgr::Initialize()
{
	l_pImplementation = new pqtIntCallbackMgr;
	prtyIntCallbackMgr::SetImplementation(l_pImplementation);
}

//---------------------------------------------------------------------------
//	DeInitialize()
//---------------------------------------------------------------------------
void pqtControlMgr::DeInitialize()
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
void pqtControlMgr::AddControlFactory(pqtControlFactory* i_pControlFactory)
{
	l_Factories.push_back(i_pControlFactory);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtControlMgr::RemoveControlFactory(pqtControlFactory* i_pControlFactory)
{
	envSTLHelpers::RemoveOneValue(l_Factories, i_pControlFactory);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int pqtControlMgr::GetNumControls()
{
	return l_Controls.size();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//pqtControl* pqtControlMgr::GetControl(const std::string& i_ControlName)
//{
//	return NULL;
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtControl* pqtControlMgr::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 QWidget* i_pParent)
{	
	DBG_ASSERT(i_pUIInfo != 0, "Cannot create a control with a NULL property UI Info");

	std::vector<pqtControlFactory*>::iterator it;
	for (it = l_Factories.begin(); it != l_Factories.end(); ++it)
	{
		pqtControlFactory *pFactory = (*it);

		pqtControl *pControl = pFactory->CreateControl(i_pUIInfo, i_pParent);
		if (pControl != NULL)
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
bool pqtControlMgr::DeleteControl(QWidget* i_pControl)
{
//	return envSTLHelpers::DeleteOneValue(l_Controls, i_pControl);

	std::vector<pqtControl*>::iterator it;
	for (it = l_Controls.begin(); it != l_Controls.end(); ++it)
	{
		pqtControl *pControl = (*it);
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
void pqtControlMgr::UpdateControl( prtyPropertyUIInfo* i_pUII )
{
	std::vector<pqtControl*>::iterator it;
	for (it = l_Controls.begin(); it != l_Controls.end(); ++it)
	{
		pqtControl *pControl = (*it);
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
void pqtIntCallbackMgr::PropertyChanged(prtyProperty* i_pProperty, int i_Index)
{
	// nothing to do here in wxWidgets
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void pqtIntCallbackMgr::UpdateControl( prtyPropertyUIInfo* i_pUII )
{
	pqtControlMgr::UpdateControl( i_pUII );
}

#endif
