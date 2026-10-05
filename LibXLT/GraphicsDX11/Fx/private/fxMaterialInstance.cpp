/*****************************************************************************
**  fxMaterialInstance.cpp
\****************************************************************************/

#include "GraphicsDX11/Fx/fxMaterialInstance.hpp"
#include "GraphicsDX11/Fx/fxEffectDX11.hpp"

#include <cstring>

const char* const fxMaterialInstance::k_MaterialBuffer = "MaterialParams";

fxMaterialInstance::fxMaterialInstance(fxEffectDX11* i_pEffect, const std::string& i_Buffer)
:	m_pEffect(i_pEffect),
	m_Buffer(i_pEffect ? i_pEffect->FindConstantBuffer(i_Buffer) : -1)
{
	if (m_Buffer < 0)
		return;
	const uint8_t* defaults = (const uint8_t*)m_pEffect->GetConstantBufferDefaults(m_Buffer);
	m_Data.assign(defaults, defaults + m_pEffect->GetConstantBufferSize(m_Buffer));
}

void fxMaterialInstance::Restore() const
{
	if (m_Buffer >= 0)
		m_pEffect->SetConstantBufferData(m_Buffer, m_Data.data(), (uint32_t)m_Data.size());
}

void fxMaterialInstance::Capture()
{
	if (m_Buffer >= 0)
		memcpy(m_Data.data(), m_pEffect->GetConstantBufferData(m_Buffer), m_Data.size());
}
