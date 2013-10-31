/*****************************************************************************
**  effShaderParamsDX11.cpp
**
**      effShaderParamsDX11
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Area18/mat/matShaderParamsGL.hpp"

#include "Core/env/envSTLHelpers.hpp"

bool matFloatBindingGL::Bind(shdrPipeline* i_pShader)
{
	glUniform1f(m_Handle, (m_Param.GetProperty().GetValue()));
	return true;
}

bool matIntBindingGL::Bind(shdrPipeline* i_pShader)
{
	glUniform1i(m_Handle, (m_Param.GetProperty().GetValue()));
	return true;
}
bool matBoolBindingGL::Bind(shdrPipeline* i_pShader)
{
	glUniform1i(m_Handle, (m_Param.GetProperty().GetValue()?1:0));
	return true;
}
bool matColorBindingGL::Bind(shdrPipeline* i_pShader)
{
	maFloatRGBA color = m_Param.GetProperty().GetValue();
	float col[4] = {color.GetRed(),color.GetGreen(),color.GetBlue(),color.GetAlpha()};
	glUniform4fv(m_Handle, col);
	return true;
}
bool matTextureBindingGL::Bind(shdrPipeline* i_pShader)
{
	if (m_ExistVarHandle != NULL && cgIsParameter(m_ExistVarHandle))
	{
		glUniform1i(m_ExistVarHandle, ((m_Param.GetTexture() == NULL)?0:1));
	}

	DBG_LOG("texture shader binding not done yet!");
	//HRESULT hr = m_Handle->SetResource(m_Param.GetTexture());
	return true;
}

matShaderBindingsGL::~matShaderBindingsGL()
{
	envSTLHelpers::DeleteContainer(m_BindableParams);
}
