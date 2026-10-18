/*****************************************************************************
**  vkdShader.cpp
**
**      see vkdShader.hpp; also vkdVertexLayout (vkdState.hpp)
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#include "GraphicsVulkan/vkd/vkdPipelineCache.hpp"
#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

#include <atomic>

namespace gvk
{
	namespace
	{
		//	Ids for pipeline keys: unique for the life of the process, never 0
		uint64_t next_id()
		{
			static std::atomic<uint64_t> s_Next(1);
			return s_Next++;
		}
	}

	//------------------------------------------------------------------------
	// vkdShader
	//------------------------------------------------------------------------
	std::unique_ptr<vkdShader> vkdShader::Create(vkdDevice* i_pDevice, VkShaderStageFlagBits i_Stage,
												 const void* i_pSpirv, size_t i_Size, const char* i_Entry,
												 const char* i_Name, std::string& o_Error)
	{
		std::unique_ptr<vkdShader> shader(new vkdShader());
		shader->m_Id = next_id();
		shader->m_Stage = i_Stage;
		shader->m_Entry = i_Entry;
		shader->m_Name = i_Name;
		if (!vkdReflect(i_pSpirv, i_Size, shader->m_Reflection, o_Error))
		{
			o_Error = shader->m_Name + ": " + o_Error;
			return nullptr;
		}
		if (shader->m_Reflection.m_Stage != i_Stage)
		{
			o_Error = shader->m_Name + ": the SPIR-V is for another stage";
			return nullptr;
		}
		if (i_pDevice)
		{
			VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
			info.codeSize = i_Size;
			info.pCode = static_cast<const uint32_t*>(i_pSpirv);
			if (VK_CHECK(vkCreateShaderModule(i_pDevice->Device(), &info, NULL, &shader->m_Module)) != VK_SUCCESS)
			{
				o_Error = shader->m_Name + ": vkCreateShaderModule failed";
				return nullptr;
			}
			i_pDevice->SetName(VK_OBJECT_TYPE_SHADER_MODULE, reinterpret_cast<uint64_t>(shader->m_Module), i_Name);
			shader->m_pDevice = i_pDevice;
		}
		return shader;
	}

	vkdShader::~vkdShader()
	{
		vkdPipelineCache::EvictEverywhere(m_Id);
		if (m_pDevice && m_Module)
		{
			VkDevice device = m_pDevice->Device();
			VkShaderModule module = m_Module;
			m_pDevice->DeferDestroy([device, module]()
			{
				vkDestroyShaderModule(device, module, NULL);
			});
		}
	}

	//------------------------------------------------------------------------
	// vkdVertexLayout
	//------------------------------------------------------------------------
	vkdVertexLayout::vkdVertexLayout(std::vector<vkdVertexElement> i_Elements, std::vector<uint32_t> i_Strides)
		: m_Id(next_id()), m_Elements(std::move(i_Elements)), m_Strides(std::move(i_Strides))
	{
	}

	uint32_t vkdVertexLayout::Stride(uint32_t i_Slot) const
	{
		return i_Slot < m_Strides.size() ? m_Strides[i_Slot] : 0;
	}
}
