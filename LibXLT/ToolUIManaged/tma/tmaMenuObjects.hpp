#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaMenuObjects.hpp
//**
//**      A menu object contains pointers to the menu item and ToolStrip button.
//**	It also has an ID.
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_MENUOBJECTS_HPP
//#error tmaMenuObjects multiply included
//#endif
//#define TMA_MENUOBJECTS_HPP
//
//#ifndef GUI_CONSTANTS_HPP
//#include "Tool/gui/guiConstants.hpp"
//#endif
//
//#ifdef _MANAGED
//
//using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaMenuObjects
//{
//public:
//
//
////------------------------------------------------------------------------
////	constructor
////------------------------------------------------------------------------
//tmaMenuObjects()
//:	m_pMenuItem( nullptr ),
//	m_pToolStripButton( nullptr ),
//	m_ID(-1),
//	m_pCallbackFunction( 0 ),
//	m_pUpdateFunction( 0 )
//{
//}
//
////------------------------------------------------------------------------
////	destructor
////------------------------------------------------------------------------
//~tmaMenuObjects()
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
////	MenuItem
////------------------------------------------------------------------------
//void SetMenuItem( ToolStripMenuItem ^ i_pMenuItem )
//{
//	m_pMenuItem = i_pMenuItem;
//}
//
//System::Windows::Forms::ToolStripMenuItem ^ GetMenuItem()
//{
//	return m_pMenuItem;
//}
//
////------------------------------------------------------------------------
////	ToolStripButton
////------------------------------------------------------------------------
//void SetToolStripButton( ToolStripButton ^ i_pToolStripButton )
//{
//	m_pToolStripButton = i_pToolStripButton;
//}
//ToolStripButton ^ GetToolStripButton()
//{
//	return m_pToolStripButton;
//}
//
//
////------------------------------------------------------------------------
////	SetCallback()
////------------------------------------------------------------------------
//void SetCallback( ControlCallback i_pFunction )
//{
//	m_pCallbackFunction = i_pFunction;
//}
//ControlCallback GetCallback()
//{
//	return m_pCallbackFunction;
//}
//
////------------------------------------------------------------------------
////	SetUpdateCallback()
////------------------------------------------------------------------------
//void SetUpdateCallback( ControlCallback i_pFunction )
//{
//	m_pUpdateFunction = i_pFunction;
//}
//ControlCallback GetUpdateCallback()
//{
//	return m_pUpdateFunction;
//}
//
//
//private:
//	//
//	//	member variables
//	//
//	ToolStripMenuItem ^ m_pMenuItem;
//	ToolStripButton ^	m_pToolStripButton;
//
//	int m_ID;
//
//	ControlCallback m_pCallbackFunction;
//	ControlCallback m_pUpdateFunction;
//};
//
//#endif // _MANAGED
