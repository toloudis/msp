///********************************************************************************************\
//**  tmaManagedStringUtils.cpp
//**
//**      Utilities for opening/saving files using fsLocators
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\********************************************************************************************/
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//
//#include "Core/fs/fsFileUtil.hpp"
//#include "Core/fs/fsLocator.hpp"
//#include "Core/it/itStringUtil.hpp"
//#include "Core/name/nameString.hpp"
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//using namespace System;
//
//
////============================================================================
////============================================================================
//namespace tmaManagedStringUtils
//{
////----------------------------------------------------------------------------
////	ManagedStringToLocator() - convert System::String to fsLocator
////----------------------------------------------------------------------------
//void ManagedStringToLocator(System::String ^i_Path, fsLocator& o_Locator)
//{
//	if (i_Path == nullptr) return;
//
//	int i = 0;
//	itString::CharType ch;
//	itString Name;
//	std::vector<itString> LocNames;
//
//	while (i < i_Path->Length)
//	{
//		ch = i_Path[i];
//		if ('\\' == ch)
//		{
//			//add Name to the vector of strings if it has a length
//			if (Name.GetLength())
//			{
//				LocNames.push_back(Name);
//				Name.Clear();
//			}
//		}
//		else
//		{
//			Name += ch;
//		}
//
//		++i;
//	}
//	//add the final Name to the vector of strings if it has a length
//	if (Name.GetLength())
//	{
//		LocNames.push_back(Name);
//		Name.Clear();
//	}
//
//	o_Locator.Clear();
//	for (i = 0; i < LocNames.size(); ++i)
//	{
//		o_Locator.Push(LocNames[i]);
//	}
//}
//
////----------------------------------------------------------------------------
////	ManagedStringToItString() - convert System::String to itString
////----------------------------------------------------------------------------
//void ManagedStringToItString(System::String ^i_Str, itString& o_Str)
//{
//	o_Str.Clear();
//	if (i_Str != nullptr)
//	{
//		int len = i_Str->Length;
//		for (int i=0; i<len; i++)
//			o_Str += i_Str[i];
//	}
//}
//
////----------------------------------------------------------------------------
////	ManagedStringToStdString() - convert System::String to std::string
////----------------------------------------------------------------------------
//void ManagedStringToStdString(System::String ^i_Str, std::string& o_Str)
//{
//	itString str;
//	ManagedStringToItString(i_Str, str);
//	o_Str = itStringUtil::GetStdString(str);
//}
//
////----------------------------------------------------------------------------
////	ManagedStringToNameString() - convert System::String to nameString
////----------------------------------------------------------------------------
//void ManagedStringToNameString(System::String ^i_Str, nameString& o_Str)
//{
//	std::string newstring;
//	ManagedStringToStdString( i_Str, newstring );
//	o_Str.SetString( newstring );
//}
//
////----------------------------------------------------------------------------
////	ManagedStringToStdString() - convert System::String to float
////----------------------------------------------------------------------------
//bool ManagedStringToFloat( System::String ^i_Str, float& o_Value )
//{
//	bool bRetVal = false;
//
//	if ( i_Str->Length > 0 )
//	{
//		try
//		{
//			o_Value = Convert::ToSingle( i_Str );
//			bRetVal = true;
//		}
//		catch (System::OverflowException^)
//		{
//			o_Value = 0.0f;
//		}
//		catch (System::FormatException^)
//		{
//			o_Value = 0.0f;
//		}
//		catch (System::ArgumentNullException^)
//		{
//			o_Value = 0.0f;
//		}
//	}
//
//	return bRetVal;
//}
//
//
////----------------------------------------------------------------------------
////	ManagedStringToInt() - convert System::String to int
////
////	return true is the string converts to a valid int, otherwise the
////	return data is invalid.
////----------------------------------------------------------------------------
//bool ManagedStringToInt( System::String ^i_Str, int& o_Value )
//{
//
//	bool bRetVal = false;
//
//	if ( i_Str->Length > 0 )
//	{
//		try
//		{
//			o_Value = Convert::ToInt32( i_Str );
//			bRetVal = true;
//		}
//		catch (System::OverflowException^)
//		{
//			o_Value = 0;
//		}
//		catch (System::FormatException^)
//		{
//			o_Value = 0;
//		}
//		catch (System::ArgumentNullException^)
//		{
//			o_Value = 0;
//		}
//	}
//
//	return bRetVal;
//}
//
//
////----------------------------------------------------------------------------
////	LocatorToManagedString() - convert fsLocator to System::String
////----------------------------------------------------------------------------
//System::String ^ LocatorToManagedString(const fsLocator& i_Locator)
//{
//	std::string str;
//	fsFileUtil::LocatorToANSIFilename(i_Locator, str);
//	return gcnew System::String( str.c_str() );
//}
//
////----------------------------------------------------------------------------
////	ItStringToManagedString() - convert itString to System::String
////----------------------------------------------------------------------------
//System::String ^ ItStringToManagedString(const itString& i_Str)
//{
//	itString null_terminated_string = i_Str;
//	null_terminated_string += itString::CharType(0);
//	return gcnew System::String( null_terminated_string.GetString() );
//}
//
////----------------------------------------------------------------------------
////	NameStringToManagedString() - convert nameString to System::String
////----------------------------------------------------------------------------
//System::String ^ NameStringToManagedString(const nameString& i_Str)
//{
//	return gcnew System::String( i_Str.GetString().c_str() );
//}
//
////----------------------------------------------------------------------------
////	Check if nameString equals a managed String
////----------------------------------------------------------------------------
//bool StdStringEqualsManagedString(const std::string& i_SStr, System::String ^i_Str)
//{
//	return i_Str->Equals( gcnew System::String(i_SStr.c_str()) );
//}
//
////----------------------------------------------------------------------------
////	Check if nameString equals a managed String
////----------------------------------------------------------------------------
//bool NameStringEqualsManagedString(const nameString& i_NStr, System::String ^i_Str)
//{
//	return i_Str->Equals( NameStringToManagedString(i_NStr) );
//}
//
//}	// end of namespace
//
//#endif // _MANAGED
