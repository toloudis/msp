/*****************************************************************************
**  tmaTabControlMgr.hpp
**
**      The main application TabControl manager.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef TMA_TABCONTROLMGR_HPP
#error tmaTabControlMgr.hpp multiply included
#endif
#define TMA_TABCONTROLMGR_HPP

#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "tmaManagedControlUtil.hpp"
#endif
#ifndef TMA_SYSTEM_HPP
#include "tmaSystem.hpp"
#endif


using namespace System;
using namespace System::Collections;
using namespace System::Drawing;


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
public __gc class tmaTabControlMgr
{
public:
	//ArrayList* l_pTabControls;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmaTabControlMgr()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
~tmaTabControlMgr()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void Initialize()
{
	//l_pTabControls = new ArrayList;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void DeInitialize()
{
	//delete l_pTabControls;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void AddTabControl( System::Windows::Forms::TabControl* i_pNewTC )
{
	//todo see if the control already exists by pointer or name

	// add it
	tmaSystem::g_pMainForm->Controls->Add( i_pNewTC );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void CreateTabControl( System::String* i_pNewTCName, int i_Width, int i_Height, int i_X, int i_Y )
{
	//todo see if the control already exists by name

	//
	System::Windows::Forms::TabControl* pNewTC;
	pNewTC = new System::Windows::Forms::TabControl;
	pNewTC->Text = i_pNewTCName;

	//todo set dimensions and location
	//pNewTC->;
	
	// add it
	tmaSystem::g_pMainForm->Controls->Add( pNewTC );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void CreateTabPage( System::String* i_pTCName, System::String* i_pNewTPName )
{
	System::Windows::Forms::TabControl* pTC;
	pTC = get_tabcontrol( i_pTCName );

	if ( pTC != 0 )
	{
		System::Windows::Forms::TabPage* pTP;
		pTP = get_tabpage( pTC, i_pNewTPName );

		if ( pTP == 0 )
		{
			pTP = new System::Windows::Forms::TabPage;
			pTP->Text = i_pNewTPName;

			pTC->Controls->Add( pTP );
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void DeleteTabPage( System::String* i_pTCName, System::String* i_pTPName )
{
	System::Windows::Forms::TabControl* pTC;
	pTC = get_tabcontrol( i_pTCName );

	if ( pTC != 0 )
	{
		System::Windows::Forms::TabPage* pTP;
		pTP = get_tabpage( pTC, i_pTPName );

		pTC->Controls->Remove( pTP );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void CreateTabPageButton( System::String* i_pTCName, 
									  System::String* i_pTPName,
									  System::String* i_pBName,
									  System::String* i_pBToolTip,
									  System::String* i_pBImageFile,
									  System::String* i_pBHandler )
{
	System::Windows::Forms::TabControl* pTC;
	pTC = get_tabcontrol( i_pTCName );

	if ( pTC != 0 )
	{
		System::Windows::Forms::TabPage* pTP;
		pTP = get_tabpage( pTC, i_pTPName );
		
		if ( pTP != 0 )
		{
			//todo verify the button name doesn't already exist.
			
			//	create the button
			System::Windows::Forms::Button* pButton;
			pButton = new System::Windows::Forms::Button;

			pButton->Text			= i_pBName;
			//pButton->ToolTipText	= i_pBToolTip;
			//todo image and handler

			// add the button
			pTP->Controls->Add( pButton );
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Void RemoveTabPageButton( System::String* i_pTCName, 
									  System::String* i_pTPName,
									  System::String* i_pBName )
{
	System::Windows::Forms::TabControl* pTC;
	pTC = get_tabcontrol( i_pTCName );

	if ( pTC != 0 )
	{
		System::Windows::Forms::TabPage* pTP;
		pTP = get_tabpage( pTC, i_pTPName );
		
		if ( pTP != 0 )
		{
			//todo verify the button name doesn't already exist.
			
			//	create the button
			System::Windows::Forms::Button* pButton;
			pButton = get_button( pTP, i_pBName );

			// remove the button
			pTC->Controls->Remove( pTP );
		}
	}
}

private:
//
//	local (private) functions
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Windows::Forms::TabControl* get_tabcontrol( System::String* i_tabcontrol_name )
{
	System::Windows::Forms::TabControl* pTabControl = 0;
	int count = tmaSystem::g_pMainForm->Controls->get_Count();

	int i;
	for ( i=0; i < count ; i++ )
	{
		pTabControl = dynamic_cast<System::Windows::Forms::TabControl*>(tmaSystem::g_pMainForm->Controls->get_Item( i ));
		if (	( pTabControl != 0 )
			&&	( String::CompareOrdinal( pTabControl->Text, i_tabcontrol_name ) == 0 ) )
		{
			break;
		}
	}

	return pTabControl;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Windows::Forms::TabPage* get_tabpage( System::Windows::Forms::TabControl* i_pTC, System::String* i_tabpage_name )
{
	System::Windows::Forms::TabPage* pTabPage = 0;
	int count = i_pTC->Controls->get_Count();

	int i;
	for ( i=0; i < count ; i++ )
	{
		pTabPage = dynamic_cast<System::Windows::Forms::TabPage*>(i_pTC->Controls->get_Item( i ));
		if (	( pTabPage != 0 )
			&&	( String::CompareOrdinal( pTabPage->Text, i_tabpage_name ) == 0 ) )
		{
			break;
		}
	}

	return pTabPage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static System::Windows::Forms::Button* get_button( System::Windows::Forms::TabPage* i_pTP, System::String* i_button_name )
{
	System::Windows::Forms::Button* pButton = 0;
	int count = i_pTP->Controls->get_Count();

	int i;
	for ( i=0; i < count ; i++ )
	{
		pButton = dynamic_cast<System::Windows::Forms::Button*>(i_pTP->Controls->get_Item( i ));
		if (	( pButton != 0 )
			&&	( String::CompareOrdinal( pButton->Text, i_button_name ) == 0 ) )
		{
			break;
		}
	}

	return pButton;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//void add_tabcontrol_button( tmaMenuObjects* pMI, bool i_bAddToModeTabControl, const char * i_TabControlButton_ImageDir )
//{
//	DBG_ASSERT0( pMI != 0, "Invalid Menu Item in tabcontrol button add" );
//
//	System::Windows::Forms::TabControl* pTC = 0;
//
//	if ( i_bAddToModeTabControl )
//	{
//		pTC = get_tabcontrol_modes();
//	}
//	else
//	{
//		pTC = get_tabcontrol_menuitems();
//	}
//
//	if ( pTC == 0 )
//	{
//		DBG_WARNING1( "Cannot find the menu item tabcontrol (%s)", pMI->GetMenuItem()->get_Text() );
//		return;
//	}
//
//	TabControlButton * pTCB = new TabControlButton();
//
//	pTCB->set_Style(TabControlButtonStyle::PushButton);
//	//pTCB->set_Visible( true );
//	//pTCB->set_Enabled( true );
//
//	//pTCB->Text = pMI->GetMenuItem()->Text;
//	pTCB->ToolTipText = pMI->GetMenuItem()->Text;
//
//	//	if the image filename + dir has been passed in,
//	//	create the icon for the tbb
//	//
//	if ( i_TabControlButton_ImageDir != 0 )
//	{
//		tmaManagedControlUtil::Create_TabControlButton_Image( pTCB, new System::String(i_TabControlButton_ImageDir), pTC );
//	}
//
//	//	set the tbb
//	pMI->SetTabControlButton( pTCB );
//
//	// Add the TabControlButton controls to the TabControl.
//	//
//	pTC->Buttons->Add( pMI->GetTabControlButton() );
//
//	// hook-up one and only one callback
//	//
//	//if ( pTC->Buttons->Count == 1 )
//	//{
//	//	pTCB->Parent->ButtonClick += new TabControlButtonClickEventHandler( (pMI), tmaMenuObjects::CallbackTBB );
//	//}
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void remove_tabcontrol_button( tmaMenuObjects* i_pMI )
//{
//	DBG_ASSERT0( i_pMI != 0, "cannot remove tabcontrol from null menu item" );
//
//	if ( i_pMI->GetTabControlButton() == 0 )
//		return;
//
//	System::Windows::Forms::TabControl* pTC = 0;
//
//	//	find out if the button exists in either tabcontrol + remove it.
//	//
//	pTC = get_tabcontrol_menuitems();
//	// FIX: DBG_ASSERT0( pTC != 0, "This app has no tabcontrol" );
//
//	if ( !pTC || !pTC->Buttons->Contains( i_pMI->GetTabControlButton() ) )
//	{
//		pTC = get_tabcontrol_modes();
//		// FIX: DBG_ASSERT0( pTC != 0, "This app has no tabcontrol" );
//
//		if (!pTC) return;
//	}
//
//	pTC->Buttons->Remove( i_pMI->GetTabControlButton() );
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void enable_tabcontrol_button( tmaMenuObjects* i_pMI, bool i_bEnable )
//{
//	DBG_ASSERT0( i_pMI != 0, "cannot remove tabcontrol from null menu item" );
//
//	if ( i_pMI->GetTabControlButton() == 0 )
//		return;
//
//	i_pMI->GetTabControlButton()->set_Enabled( i_bEnable );
//}


//===========================================================================
//	tmaTabControlMgr functions
//===========================================================================

//--------------------------------------------------------------------
// Execution event handler
//--------------------------------------------------------------------
//void tabcontrol_ButtonClick( Object* Sender, System::Windows::Forms::TabControlButtonClickEventArgs* e )
//{
//	System::Windows::Forms::TabControlButton* pTCB = e->Button;
//
//	if ( pTCB )
//	{
//		tmaMenuObjects* pMO = tmaMenuObjectsMgr::g_pMgr->get_menuitem( pTCB );
//
//		if ( pMO )
//		{
//			if ( pMO->GetCallback() )
//			{
//				(*(pMO->GetCallback()))( pMO->GetID() );
//			}
//		}
//	}
//
//	//	id = id + e->Button->Parent->Buttons->IndexOf(e->Button);
//	//DBG_LOG1( "press TabControl Button (%s)", e->Button->Text );
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//void AttachHandler( tmaMenuObjects* i_pMO )
//{
//	TabControlButton* tbb = i_pMO->GetTabControlButton();
//	if (tbb != 0)
//	{
//		TabControlButtonClickEventHandler* handler = new TabControlButtonClickEventHandler( this, tabcontrol_ButtonClick );
//		tbb->Parent->ButtonClick -= handler;
//		tbb->Parent->ButtonClick += handler;
//	}
//}

};