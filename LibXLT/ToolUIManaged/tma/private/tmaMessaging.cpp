///*****************************************************************************
//**  tmaMessaging.cpp
//**
//**      see .hpp
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#include <windows.h>
//
//#include "ToolUIManaged/tma/tmaMessaging.hpp"
//
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#include "Tool/cma/cmaCommandMgr.hpp"
//#include <string>
//
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//using namespace System;
//using namespace System::ComponentModel;
//using namespace System::Windows::Forms;
//
//
////------------------------------------------------------------------------
////------------------------------------------------------------------------
//bool tmaMessaging::ProcessCmdKey(Message% msg, Keys keyData)
//{
//	if ((msg.Msg == WM_KEYDOWN) || (msg.Msg == WM_SYSKEYDOWN))
//	{
//		//
//		//	try to display a "legal" key combo string
//		//
//		int keyCode = (int) keyData;
//		int origKeyCode = keyCode;
//		String^ keycombo;
//		KeysConverter^ kc = gcnew KeysConverter();
//
//		keycombo = kc->ConvertToString(keyData);
//		//string shortcut= TypeDescriptor.GetConverter(typeof(Keys)).ConvertToString((Keys)mi.Shortcut);
//		//TypeDescriptor.GetConverter(GetType(Keys)).ConvertToString(k)
//		//	Get the keys
//		try
//		{
//			//	try to convert it and see if it throws an exception
//			//
//			TypeConverter^ keyconv = TypeDescriptor::GetConverter(Keys::typeid);
//			//keycombo = keyconv->ConvertToString(keyData);
//			//Shortcut scut = (Shortcut)keyconv.ConvertFromString(keycombo);
//		}
//		catch (ArgumentNullException^)
//		{
//			// if not a valid key combo, show nothing.
//			keycombo = nullptr;
//		}
//		catch (ArgumentException^)
//		{
//			// if not a valid key combo, show nothing.
//			keycombo = nullptr;
//		}
//
//		//	execute the command
//		//
//		if (keycombo != nullptr)
//		{
//			std::string strKeyCombo;
//			tmaManagedStringUtils::ManagedStringToStdString(keycombo, strKeyCombo);
//			cmaCommandMgr::ExecuteCommand(strKeyCombo);
//	
//			//this->set_Text( keycombo );
//		}
//
//		// Return true to "swallow" the key.
//		return true;    
//	}
//
//	return false;
//	//return (__super::ProcessCmdKey(msg,keyData));
//}
//#endif // _MANAGED
