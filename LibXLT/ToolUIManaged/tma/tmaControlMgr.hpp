#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaControlMgr.hpp
//**
//**      The manager for the tmaControl class
//**
//**	StudioGPU
//**	Copyright(C) 2005 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_CONTROLMGR_HPP
//#error tmaControlMgr.hpp multiply included
//#endif
//#define TMA_CONTROLMGR_HPP
//
//#ifndef TMA_CONTROL_HPP
//#include "ToolUIManaged/tma/tmaControl.hpp"
//#endif
//
//#ifndef CMA_COMMAND_HPP
//#include "Tool/cma/cmaCommand.hpp"
//#endif
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System::Collections;
//
//
////============================================================================
////============================================================================
//public ref class tmaControlMgr
//{
//public:
//	static tmaControlMgr^ g_pMgr = nullptr;
//
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaControlMgr()
//	{
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	~tmaControlMgr()
//	{
//	}
//
//
//	//
//	//	local (private) functions
//	//
//
//	//---------------------------------------------------------------------------
//	//	generate a menu item
//	//---------------------------------------------------------------------------
//	static tmaControl^ generate_control()
//	{
//		tmaControl^ pCtrl = gcnew tmaControl;
//		g_pMgr->l_pControl->Add( pCtrl );
//
//		//	set the ID
//		//
//		// HACK: - the 200 is arbitrary.  This is trying to avoid having the same
//		//	ID as another menu item command.  There should probably be some
//		//	utility that creates a unique ID.
//		//
//		pCtrl->SetID(200 + (g_pMgr->l_pControl->Count - 1));
//
//		return pCtrl;
//	}
//
//	//---------------------------------------------------------------------------
//	//	get a menu item
//	//---------------------------------------------------------------------------
//	tmaControl^ get_control( int i_ObjectID )
//	{
//		tmaControl^ pCtrl;
//
//		System::Collections::IEnumerator^ myEnumerator = l_pControl->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pCtrl = dynamic_cast<tmaControl^>(myEnumerator->Current);
//			if ( pCtrl->GetID() == i_ObjectID )
//			{
//				return pCtrl;
//			}
//		}
//		return nullptr;
//	}
//
//	//---------------------------------------------------------------------------
//	//	get a Control item from a Control item
//	//---------------------------------------------------------------------------
//	tmaControl^ get_control( Control^ i_pControl )
//	{
//		tmaControl^ pCtrl;
//
//		System::Collections::IEnumerator^ myEnumerator = l_pControl->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pCtrl = dynamic_cast<tmaControl^>(myEnumerator->Current);
//			if ( pCtrl->GetControl() == i_pControl )
//			{
//				return pCtrl;
//			}
//		}
//		return nullptr;
//	}
//
//
//	//===========================================================================
//	//	tmaControlMgr functions
//	//===========================================================================
//
//	//---------------------------------------------------------------------------
//	//	Initialize()
//	//---------------------------------------------------------------------------
//	void Initialize()
//	{
//		l_pControl = gcnew ArrayList;
//		l_pControl->Clear();
//	}
//
//	//---------------------------------------------------------------------------
//	//	DeInitialize()
//	//---------------------------------------------------------------------------
//	void DeInitialize()
//	{
//		l_pControl->Clear();
//		//delete l_pControl;
//	}
//
//	//---------------------------------------------------------------------------
//	//	Add()
//	//---------------------------------------------------------------------------
//	static int Add( Control^ pCtrl, cmaCommand* pCmd )
//	{
//		tmaControl^ ptmaCtrl = generate_control();
//
//		ptmaCtrl->SetControl( pCtrl );
//		ptmaCtrl->SetCommand( pCmd );
//
//		AttachCallbacks( ptmaCtrl->GetID() );
//
//		return ptmaCtrl->GetID();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	static void AttachCallbacks( int i_ID )
//	{
//		tmaControlMgr::g_pMgr->AttachCallbacksNonStatic(i_ID);
//	}
//	void AttachCallbacksNonStatic( int i_ID )
//	{
//		tmaControl^ pCtrl = this->get_control( i_ID );
//		DBG_ASSERT0( pCtrl != nullptr, "control is 0" );
//
//		//	set the click event
//		pCtrl->GetControl()->Click += gcnew System::EventHandler(this, &tmaControlMgr::control_Click);
//		//pCtrl->GetControl()->Popup += gcnew System::EventHandler(this, &tmaControlMgr::control_Popup);
//	}
//
//	//---------------------------------------------------------------------------
//	//	AttachEventToControl()
//	//---------------------------------------------------------------------------
//	//void AttachEventToControl( int i_ObjectID, ControlCallback i_pFunction, ControlCallback i_pUpdate )
//	//{
//	//	tmaControl^ pCtrl = tmaControlMgr::g_pMgr->get_control( i_ObjectID );
//	//	DBG_ASSERT0( pCtrl != nullptr, "control is 0" );
//	//
//	//	pCtrl->SetCallback( i_pFunction );
//	//	if (i_pUpdate != nullptr)
//	//		pCtrl->SetUpdateCallback( i_pUpdate );
//	//
//	//	//	set the click event
//	//	pCtrl->GetControl()->Click += gcnew System::EventHandler(pCtrl->GetControl(), tmaControlMgr::control_Click);
//	//	pCtrl->GetControl()->Popup += gcnew System::EventHandler(pCtrl->GetControl(), tmaControlMgr::control_Popup);
//	//}
//
//	//---------------------------------------------------------------------------
//	//	GetControlList()
//	//---------------------------------------------------------------------------
//	ArrayList^ GetControlList()
//	{
//		return l_pControl;
//	}
//
//
//	//---------------------------------------------------------------------------
//	// Execution event handler
//	//---------------------------------------------------------------------------
//	void control_Click( Object^ Sender, System::EventArgs^ e )
//	{
//		//if (dynamic_cast<Button^>(Sender))
//		//{
//		//	System::Windows::Forms::MessageBox::Show( "button control clicked!" );
//		//}
//
//		if (dynamic_cast<Button^>(Sender))
//		{
//			tmaControl^ pCtrl = tmaControlMgr::g_pMgr->get_control( dynamic_cast<Button^>(Sender) );
//			if ( pCtrl )
//			{
//				pCtrl->OnExecute();
//			}
//		}
//	}
//
//
//	//---------------------------------------------------------------------------
//	// Execution event handler
//	//---------------------------------------------------------------------------
//	void control_Popup( Object^ Sender, System::EventArgs^ e )
//	{
//		//if (dynamic_cast<Button^>(Sender))
//		//{
//		//	System::Windows::Forms::MessageBox::Show( "button control clicked!" );
//		//}
//
//		if (dynamic_cast<Button^>(Sender))
//		{
//			tmaControl^ pCtrl = tmaControlMgr::g_pMgr->get_control( dynamic_cast<Button^>(Sender) );
//			if ( pCtrl )
//			{
//				pCtrl->OnUpdate();
//			}
//		}
//	}
//		//---------------------------------------------------------------------------
//		//---------------------------------------------------------------------------
//		void SetIconDirectory( const char * i_IconDirectory )
//		{
//			if ( m_IconDir == nullptr )
//			{
//				m_IconDir = gcnew System::String( i_IconDirectory );
//			}
//			else
//			{
//				m_IconDir->Copy( gcnew System::String(i_IconDirectory) );
//			}
//		}
//
//private:
//	System::String^	m_IconDir;
//
//private:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	ArrayList^ l_pControl;	// tmaControl
//};
//
//#endif // _MANAGED
