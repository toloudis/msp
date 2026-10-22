/*****************************************************************************
**  effShaderParamsVk.cpp
**
**	See effShaderParamsVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_SHADERPARAMSVK_HPP
#include "GraphicsVulkan/Eff/effShaderParamsVk.hpp"
#endif

#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif

namespace gvk
{

bool effFloatBindingVk::Bind(fxEffect* i_pShader)
{
	bool hr = m_Handle->SetFloat(m_Param.GetProperty().GetValue());
	if (!hr)
	{
		DBG_LOG("could not bind float param " << m_Param.GetName());
	}
	return hr;
}

bool effIntBindingVk::Bind(fxEffect* i_pShader)
{
	bool hr = m_Handle->SetInt(m_Param.GetProperty().GetValue());
	if (!hr)
	{
		DBG_LOG("could not bind int param " << m_Param.GetName());
	}
	return hr;
}
bool effBoolBindingVk::Bind(fxEffect* i_pShader)
{
	bool hr = m_Handle->SetBool(m_Param.GetProperty().GetValue()?true:false);
	if (!hr)
	{
		DBG_LOG("could not bind bool param " << m_Param.GetName());
	}
	return hr;
}
bool effColorBindingVk::Bind(fxEffect* i_pShader)
{
	maFloatRGBA color = m_Param.GetProperty().GetValue();
	float col[4] = {color.GetRed(),color.GetGreen(),color.GetBlue(),color.GetAlpha()};
	bool hr = m_Handle->SetFloatVector(col);
	if (!hr)
	{
		DBG_LOG("could not bind color param " << m_Param.GetName());
	}
	return hr;
}
bool effTextureBindingVk::Bind(fxEffect* i_pShader)
{
	vkdImage* pTex = g3dVkTextureUtil::GetImage(m_Param.GetTexture());
	if (m_ExistVarHandle != NULL && m_ExistVarHandle->IsValid())
	{
		bool hr = m_ExistVarHandle->SetBool((pTex == NULL)?false:true);
		if (!hr)
			DBG_LOG("could not bind texture exists param " << m_Param.GetName());
	}
	bool hr = m_Handle->SetResource(pTex);
	if (!hr)
	{
		DBG_LOG("could not bind texture param " << m_Param.GetName());
	}
	return hr;
}

effShaderBindingsVk::~effShaderBindingsVk()
{
	envSTLHelpers::DeleteContainer(m_BindableParams);
}

}
