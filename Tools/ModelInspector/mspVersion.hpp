/*****************************************************************************
**	mspVersion.hpp
**
**	 Holds version info for application
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_VERSION_HPP
#error mspVersion.hpp multiply included
#endif
#define MSP_VERSION_HPP

//============================================================================
//============================================================================
//namespace mspVersion
//{
//	const char* c_AssemblyTitle = "ModelInspector";
//	const char* c_AssemblyVersion = "1.0.0.0";
//}

// Had to use defines in order to handle System::String requirements in .NET
#define c_AssemblyTitle "ModelInspector"
#define c_AssemblyVersion "1.0.5.0"
