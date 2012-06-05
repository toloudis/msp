/*****************************************************************************
**  matMetaFX.hpp
**
**    This object contains the data for an effects file in multiple
**	formats. Some portions may be compiled to a specific language and
**	some parts may be in a Meta language (like MetaSL).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_METAFX_HPP
#error matMetaFX.hpp multiply included
#endif
#define MAT_METAFX_HPP

#ifndef ENV_APPVERSION_HPP
#include "Core/Env/envAppVersion.hpp"
#endif 

#include <string>
#include <vector>

template<class DataType>
class matMetaFXBlock
{
public:
	//====================================================================
	//====================================================================
	matMetaFXBlock() : m_bHasData(false) {}

public:
	bool m_bHasData;
	envAppVersion m_Version;
	DataType m_Data;
};

class matMetaFX 
{
public:
	//====================================================================
	//====================================================================
	matMetaFX();

	//====================================================================
	//====================================================================
	virtual ~matMetaFX();

	//====================================================================
	// Get current version of shaders to use when compiling a HLSL shader
	//====================================================================
	static envAppVersion GetCurrentFXVersion();
	static envAppVersion GetCurrentHLSLVersion();
	static envAppVersion GetCurrentMetaSLVersion();

	//====================================================================
	// Compare given version against reference version, returning true
	// if the given version can be used.
	//====================================================================
	static bool CompareVersion(const envAppVersion &i_Version,
							   const envAppVersion & i_ReferenceVersion);

	//====================================================================
	// Set compiled FX data from buffer and size. 
	//	This class will make a copy of the data,
	//====================================================================
	void SetCompiledFXData(envType::UInt8 * i_CompiledShader, 
						 int i_CompiledShaderSize);
	
	//====================================================================
	// Set HLSL shader source from string.
	// This class will make a copy of the data,
	//====================================================================
	void SetHLSLSource(const std::string& i_ShaderSource);

	//====================================================================
	// Set MetaSL shader source from string.
	// This class will make a copy of the data,
	//====================================================================
	void SetMetaSLSource(const std::string& i_ShaderSource);

	std::string m_ShaderName;
	matMetaFXBlock< std::vector<envType::UInt8> > m_CompiledFX;
	matMetaFXBlock< std::string > m_HLSLSource;
	matMetaFXBlock< std::string > m_MetaSLSource;
};
