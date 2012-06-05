/*****************************************************************************
**	ControlsTestMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ControlsTestMgr.hpp"

#include "ControlsTestDialogUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyObject.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"


//============================================================================
//============================================================================
namespace ControlsTestMgr
{
	//===========================================================================
	//	general namespace
	//===========================================================================
	namespace
	{
		//==========================================================================
		//	Load Prefs base object
		//==========================================================================
		class ControlsTestObject : public prtyObject
		{
			public:
				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				ControlsTestObject();

			private:
				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				void UpdateBooleanTest(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateNumberTest(prtyProperty *i_pProperty, bool i_bDirty);

			public:
				ControlsTestData m_Data;
		};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		ControlsTestObject::ControlsTestObject()
		{
			prtyPropertyUIInfo* pPUII;
			prtyNumericUpDownUIInfo* pNUDUII;

			// Test properties
			//
			pPUII = new prtyButtonUIInfo(&(m_Data.m_bTriggerTest), "Trigger", "Test out property Trigger");
			AddProperty( pPUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bBooleanTest), "Boolean", "Test out property Boolean");
			AddProperty( pPUII );

			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Int8Test), "Numbers", "Test out property Int8");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(10);
			AddProperty( pNUDUII );

			pPUII = new prtyFloatEditUIInfo(&(m_Data.m_Int32Test), "Numbers", "Test out property Int32");
			//pPUII->SetDecimalPlaces(0);
			AddProperty( pPUII );

			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_FloatTest), "Numbers", "Test out property Float");
			pNUDUII->SetDecimalPlaces(2);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(20);
			AddProperty( pNUDUII );

			// Add Callbacks
			//
			m_Data.m_bBooleanTest.AddCallback(new prtyCallbackWrapper<ControlsTestObject>(this, &ControlsTestObject::UpdateBooleanTest));
			m_Data.m_Int8Test.AddCallback(new prtyCallbackWrapper<ControlsTestObject>(this, &ControlsTestObject::UpdateNumberTest));
			m_Data.m_Int32Test.AddCallback(new prtyCallbackWrapper<ControlsTestObject>(this, &ControlsTestObject::UpdateNumberTest));
			m_Data.m_FloatTest.AddCallback(new prtyCallbackWrapper<ControlsTestObject>(this, &ControlsTestObject::UpdateNumberTest));

			// Note: some callbacks are not needed because the system code will look at
			// the preferences data itself. (Like projected lights will look at the depth
			// map settings.) So, we only need callbacks for Terawatt code values.
		}

		void ControlsTestObject::UpdateBooleanTest(prtyProperty *i_pProperty, bool i_bDirty)
		{
			DBG_LOG("Boolean Test Update");
		}
		void ControlsTestObject::UpdateNumberTest(prtyProperty *i_pProperty, bool i_bDirty)
		{
			DBG_LOG("Number Test Update");
		}

		//==========================================================================
		//	property object
		//==========================================================================
		ControlsTestObject* l_pPrefObject = NULL;

		//====================================================================
		//====================================================================
#ifdef QT_FINISH_PORT
		class ControlsTestNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve strings into our render pref property objects
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("ControlsTest"))
				{
					return l_pPrefObject;
				}
				return NULL;
			}
		};

		shared_ptr<ControlsTestNameResolver> l_NameResolver(new ControlsTestNameResolver);
#endif
	}


	//========================================================================
	//	ControlsTestMgr functions
	//========================================================================

	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void Init()
	{
		//	create the prefs objects if not already created
		//
		if (l_pPrefObject == NULL)
		{
			l_pPrefObject = new ControlsTestObject();

			// Add name resolver so python can access the render preferences
#ifdef QT_FINISH_PORT
			pythProperty::AddNameResolver( l_NameResolver ); 
#endif

			// create commands to display dialog
			AddToMenu();
		}
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pPrefObject != NULL)
		{
			// Remove name resolver 
#ifdef QT_FINISH_PORT
			pythProperty::RemoveNameResolver( l_NameResolver ); 
#endif
			// LEAK!
#ifdef QT_FINISH_PORT
			delete l_pPrefObject;
			l_pPrefObject = NULL;
#endif
		}
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Prefs actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: View Preferences
#ifdef QT_FINISH_PORT
		pCmd = new cmaCommandSimple("Test Something", 
									"Test", 
									"Test out something important",
									&ControlsTestDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#endif
	}

	//------------------------------------------------------------------------
	// Access to data structure with properties
	//------------------------------------------------------------------------
	ControlsTestData& Data()
	{
		DBG_ASSERT(l_pPrefObject != NULL, "Load Prefs object not created yet.");
		return l_pPrefObject->m_Data;
	}

	//------------------------------------------------------------------------
	// Access to property object
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pPrefObject;
	}

}
