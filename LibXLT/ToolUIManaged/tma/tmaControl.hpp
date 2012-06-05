#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaControl.hpp
//**
//**      A control contains pointers to the .NET control.
//**	It also has an ID.
//**
//**	StudioGPU
//**	Copyright(C) 2005 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_CONTROL_HPP
//#error tmaControl multiply included
//#endif
//#define TMA_CONTROL_HPP
//
//#ifndef GUI_CONSTANTS_HPP
//#include "Tool/gui/guiConstants.hpp"
//#endif
//#ifndef CMA_COMMAND_HPP
//#include "Tool/cma/cmaCommand.hpp"
//#endif
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaControl
//{
//public:
//
//
////------------------------------------------------------------------------
////	constructor
////------------------------------------------------------------------------
//tmaControl()
//:	m_pControl( nullptr ),
//	m_pCommand( 0 ),
//	m_ID(-1)
//{
//}
//
////------------------------------------------------------------------------
////	destructor
////------------------------------------------------------------------------
//~tmaControl()
//{
//}
//
////------------------------------------------------------------------------
////	ID
////------------------------------------------------------------------------
//void SetID( int i_ID )
//{
//	m_ID = i_ID;
//}
//int GetID()
//{
//	return m_ID;
//}
//
////------------------------------------------------------------------------
////	Control
////------------------------------------------------------------------------
//void SetControl( Control^ i_pControl )
//{
//	m_pControl = i_pControl;
//}
//
//System::Windows::Forms::Control^ GetControl()
//{
//	return m_pControl;
//}
//
////------------------------------------------------------------------------
////	Command
////------------------------------------------------------------------------
//void SetCommand( cmaCommand * i_pCommand )
//{
//	m_pCommand = i_pCommand;
//}
//
//cmaCommand * GetCommand()
//{
//	return m_pCommand;
//}
//
////------------------------------------------------------------------------
////------------------------------------------------------------------------
//void OnExecute()
//{
//	m_pCommand->Execute();
//}
//
////------------------------------------------------------------------------
////------------------------------------------------------------------------
//void OnUpdate()
//{
//	m_pCommand->ProcessUpdates();
//}
//
//private:
//	//
//	//	member variables
//	//
//	Control ^	m_pControl;
//	cmaCommand*	m_pCommand;
//
//	int m_ID;
//};
//
//#endif // _MANAGED
