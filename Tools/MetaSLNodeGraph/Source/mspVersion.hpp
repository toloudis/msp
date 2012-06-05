/*****************************************************************************
**	mspVersion.hpp
**
**	 Holds version info for application
**
**	StudioGPU
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
#define c_AssemblyTitle "Shader Node Graph Editor"
#define c_AssemblyVersion "1.0.0.3"
