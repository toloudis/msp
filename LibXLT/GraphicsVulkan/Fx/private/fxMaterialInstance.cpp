/*****************************************************************************
**  fxMaterialInstance.cpp
\****************************************************************************/

#include "GraphicsVulkan/Fx/fxMaterialInstance.hpp"
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"

#include <cstring>

namespace gvk
{

const char* const fxMaterialInstance::k_MaterialBuffer = "MaterialParams";

fxMaterialInstance::fxMaterialInstance(fxEffectVk* i_pEffect, const std::string& i_Buffer)
:	m_pEffect(i_pEffect),
	m_Buffer(i_pEffect ? i_pEffect->FindConstantBuffer(i_Buffer) : -1)
{
	if (m_Buffer < 0)
		return;
	const uint8_t* defaults = (const uint8_t*)m_pEffect->GetConstantBufferDefaults(m_Buffer);
	m_Data.assign(defaults, defaults + m_pEffect->GetConstantBufferSize(m_Buffer));
}

void fxMaterialInstance::Exclude(const std::string& i_Variable)
{
	if (m_Buffer < 0)
		return;
	const fxEffectVk::Constant constant = m_pEffect->FindConstant(i_Variable);
	if (constant.m_Buffer == m_Buffer)
		m_Excluded.push_back(std::make_pair(constant.m_Offset, constant.m_Size));
}

void fxMaterialInstance::Restore() const
{
	if (m_Buffer < 0)
		return;
	if (m_Excluded.empty())
	{
		m_pEffect->SetConstantBufferData(m_Buffer, m_Data.data(), (uint32_t)m_Data.size());
		return;
	}
	// the renderer's variables keep what is in the effect's buffer now
	std::vector<uint8_t> data = m_Data;
	const uint8_t* current = (const uint8_t*)m_pEffect->GetConstantBufferData(m_Buffer);
	for (const std::pair<uint32_t, uint32_t>& range : m_Excluded)
		memcpy(data.data() + range.first, current + range.first, range.second);
	m_pEffect->SetConstantBufferData(m_Buffer, data.data(), (uint32_t)data.size());
}

void fxMaterialInstance::Capture()
{
	if (m_Buffer >= 0)
		memcpy(m_Data.data(), m_pEffect->GetConstantBufferData(m_Buffer), m_Data.size());
}
}
