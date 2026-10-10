/*****************************************************************************
**  effShaderParamsDX11.cpp
**
**      effShaderParamsDX11
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effShaderParamsDX11.hpp"

#include "Core/env/envSTLHelpers.hpp"

bool effFloatBindingDX11::Bind(fxEffect* i_pShader)
{
	HRESULT hr = m_Handle->SetFloat(m_Param.GetProperty().GetValue());
	if (!SUCCEEDED(hr))
	{
		DBG_LOG("could not bind float param " << m_Param.GetName());
	}
	return (SUCCEEDED(hr));
}

bool effIntBindingDX11::Bind(fxEffect* i_pShader)
{
	HRESULT hr = m_Handle->SetInt(m_Param.GetProperty().GetValue());
	if (!SUCCEEDED(hr))
	{
		DBG_LOG("could not bind int param " << m_Param.GetName());
	}
	return (SUCCEEDED(hr));
}
bool effBoolBindingDX11::Bind(fxEffect* i_pShader)
{
	HRESULT hr = m_Handle->SetBool(m_Param.GetProperty().GetValue()?TRUE:FALSE);
	if (!SUCCEEDED(hr))
	{
		DBG_LOG("could not bind bool param " << m_Param.GetName());
	}
	return (SUCCEEDED(hr));
}
bool effColorBindingDX11::Bind(fxEffect* i_pShader)
{
	maFloatRGBA color = m_Param.GetProperty().GetValue();
	float col[4] = {color.GetRed(),color.GetGreen(),color.GetBlue(),color.GetAlpha()};
	HRESULT hr = m_Handle->SetFloatVector(col);
	if (!SUCCEEDED(hr))
	{
		DBG_LOG("could not bind color param " << m_Param.GetName());
	}
	return (SUCCEEDED(hr));
}
bool effTextureBindingDX11::Bind(fxEffect* i_pShader)
{
	ID3D11ShaderResourceView* pTex = g3dDX11TextureUtil::GetD3DTexture(m_Param.GetTexture());
	if (m_ExistVarHandle != NULL && m_ExistVarHandle->IsValid())
	{
		HRESULT hr = m_ExistVarHandle->SetBool((pTex == NULL)?FALSE:TRUE);
		if (!SUCCEEDED(hr))
			g2dDX11Global::PrintDXError(hr);
	}
	HRESULT hr = m_Handle->SetResource(pTex);
	if (!SUCCEEDED(hr))
	{
		DBG_LOG("could not bind texture param " << m_Param.GetName());
	}
	return (SUCCEEDED(hr));
}

effShaderBindingsDX11::~effShaderBindingsDX11()
{
	envSTLHelpers::DeleteContainer(m_BindableParams);
}
