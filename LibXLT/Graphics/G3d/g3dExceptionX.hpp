/****************************************************************************\
**	g3dExceptionX.hpp
**
**		g3dExceptionX.hpp defines the exceptions that can be thrown from the
**	g3d package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_EXCEPTIONX_HPP
#error g3dExceptionX.hpp multiply included
#endif
#define G3D_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif


//============================================================================
//	g3dShaderLoadX is thrown when a shader can not be loaded.
//============================================================================
class g3dShaderLoadX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g3dShaderLoadX(std::string i_ShaderName);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;

		//------------------------------------------------------------------------
		// GetNodeName
		//------------------------------------------------------------------------
		std::string GetShaderName() const;

	private:
		std::string m_ShaderName;
};

