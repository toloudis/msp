/*****************************************************************************
**  matMetaFX.cpp
**
**    see hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matMetaFX.hpp"



//====================================================================
//====================================================================
matMetaFX::matMetaFX()
{
}

//====================================================================
//====================================================================
matMetaFX::~matMetaFX()
{
}


//====================================================================
// Get current version of shaders to use when compiling a HLSL shader
//====================================================================
envAppVersion matMetaFX::GetCurrentFXVersion()
{
	// This version should be incremented when something in the supporting 
	// shaders (i.e. Lighting.h, Support.h) changes so that user shaders
	// will be recompiled with the new headers.
	return envAppVersion(1,0,0,0);
}
envAppVersion matMetaFX::GetCurrentHLSLVersion()
{
	// This version should be incremented if the interface between the
	// header and footer and the HLSL shade function changes. If the HLSL code
	// comes from an older version, then this would force a recompile from 
	// the MetaSL version of code or an exception to be thrown.
	// This version should not have to be incremented often.
	return envAppVersion(1,0,0,0);
}
envAppVersion matMetaFX::GetCurrentMetaSLVersion()
{
	// This version marks the version of MetaSL code and 
	// is not currently used.
	return envAppVersion(1,0,0,0);
}

//====================================================================
// Compare given version against reference version, returning true
// if the given version can be used.
//====================================================================
bool matMetaFX::CompareVersion(const envAppVersion &i_Version,
							   const envAppVersion & i_ReferenceVersion)
{
	// We want to only compare the first 3 numbers of the version in this case,
	// the fourth number is allowed to increment for reference purposes 
	// without invalidating. 
	return ((i_Version.GetMajor() == i_ReferenceVersion.GetMajor()) &&
			(i_Version.GetMinor() == i_ReferenceVersion.GetMinor()) &&
			(i_Version.GetRevision() == i_ReferenceVersion.GetRevision()));
}

//====================================================================
// Set compiled FX data from buffer and size. This class will 
// make a copy of the data,
//====================================================================
void matMetaFX::SetCompiledFXData(envType::UInt8 * i_CompiledShader, 
								  int i_CompiledShaderSize)
{
	if (i_CompiledShaderSize > 0)
	{
		m_CompiledFX.m_Data.resize(i_CompiledShaderSize);
		m_CompiledFX.m_Version = matMetaFX::GetCurrentFXVersion();
		::memcpy(&m_CompiledFX.m_Data[0], i_CompiledShader, i_CompiledShaderSize);
		m_CompiledFX.m_bHasData = true;
	}
}

//====================================================================
// Set HLSL shader source from string.
// This class will make a copy of the data,
//====================================================================
void matMetaFX::SetHLSLSource(const std::string& i_ShaderSource)
{
	if (!i_ShaderSource.empty())
	{
		m_HLSLSource.m_Data = i_ShaderSource;
		m_HLSLSource.m_Version = matMetaFX::GetCurrentHLSLVersion();
		m_HLSLSource.m_bHasData = true;
	}
}

//====================================================================
// Set MetaSL shader source from string.
// This class will make a copy of the data,
//====================================================================
void matMetaFX::SetMetaSLSource(const std::string& i_ShaderSource)
{
	if (!i_ShaderSource.empty())
	{
		m_MetaSLSource.m_Data = i_ShaderSource;
		m_MetaSLSource.m_Version = matMetaFX::GetCurrentMetaSLVersion();
		m_MetaSLSource.m_bHasData = true;
	}
}


