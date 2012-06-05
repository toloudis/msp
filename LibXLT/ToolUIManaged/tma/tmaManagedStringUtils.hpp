//#error THIS_FILE_IS_OBSOLETE

///********************************************************************************************\
//**  tmaManagedStringUtils.hpp
//**
//**      Utilities for opening/saving files using fsLocators
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\********************************************************************************************/
//#ifdef	TMA_MANAGEDSTRINGUTILS_HPP
//#error	tmaManagedStringUtils.hpp included recursively.
//#endif
//#define	TMA_MANAGEDSTRINGUTILS_HPP
//
//#include <string>
//
//
////============================================================================================
////	forward references
////============================================================================================
//class fsLocator;
//class itString;
//class nameString;
//#ifdef _MANAGED
//
//
////============================================================================================
////	tmaManagedStringUtils Functions
////============================================================================================
//namespace tmaManagedStringUtils
//{
//	//----------------------------------------------------------------------------
//	//	ManagedStringToLocator() - convert System::String to fsLocator
//	//----------------------------------------------------------------------------
//	void ManagedStringToLocator(System::String ^i_Path, fsLocator& o_Locator);
//
//	//----------------------------------------------------------------------------
//	//	ManagedStringToItString() - convert System::String to itString
//	//----------------------------------------------------------------------------
//	void ManagedStringToItString(System::String ^i_Str, itString& o_Str);
//
//	//----------------------------------------------------------------------------
//	//	ManagedStringToStdString() - convert System::String to std::string
//	//----------------------------------------------------------------------------
//	void ManagedStringToStdString(System::String ^i_Str, std::string& o_Str);
//
//	//----------------------------------------------------------------------------
//	//	ManagedStringToNameString() - convert System::String to nameString
//	//----------------------------------------------------------------------------
//	void ManagedStringToNameString(System::String ^i_Str, nameString& o_Str);
//
//	//----------------------------------------------------------------------------
//	//	ManagedStringToFloat() - convert System::String to float
//	//
//	//	return true is the string converts to a valid float, otherwise the
//	//	return data is invalid.
//	//----------------------------------------------------------------------------
//	bool ManagedStringToFloat( System::String ^i_Str, float& o_Value );
//
//	//----------------------------------------------------------------------------
//	//	ManagedStringToInt() - convert System::String to int
//	//
//	//	return true is the string converts to a valid int, otherwise the
//	//	return data is invalid.
//	//----------------------------------------------------------------------------
//	bool ManagedStringToInt( System::String ^i_Str, int& o_Value );
//
//	//----------------------------------------------------------------------------
//	//	LocatorToManagedString() - convert fsLocator to System::String
//	//----------------------------------------------------------------------------
//	System::String ^ LocatorToManagedString(const fsLocator& i_Locator);
//
//	//----------------------------------------------------------------------------
//	//	ItStringToManagedString() - convert itString to System::String
//	//----------------------------------------------------------------------------
//	System::String ^ ItStringToManagedString(const itString& i_Str);
//
//	//----------------------------------------------------------------------------
//	//	NameStringToManagedString() - convert nameString to System::String
//	//----------------------------------------------------------------------------
//	System::String ^ NameStringToManagedString(const nameString& i_Str);
//
//	//----------------------------------------------------------------------------
//	//	Check if nameString equals a managed String
//	//----------------------------------------------------------------------------
//	bool StdStringEqualsManagedString(const std::string& i_SStr, System::String ^i_Str);
//
//	//----------------------------------------------------------------------------
//	//	Check if nameString equals a managed String
//	//----------------------------------------------------------------------------
//	bool NameStringEqualsManagedString(const nameString& i_NStr, System::String ^i_Str);
//
//};
//#endif // _MANAGED
