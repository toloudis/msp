/*****************************************************************************
**  vkdShader.hpp
**
**      vkdShader is one compiled shader stage: its VkShaderModule, the entry
**	point DXC kept (the HLSL function name), what it reads (vkdReflect)
**	and an id that pipeline keys use. Destroying it evicts every pipeline
**	built with it and destroys the module once the GPU is done with it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDSHADER_HPP
#error vkdShader.hpp multiply included
#endif
#define GVK_VKDSHADER_HPP

#ifndef GVK_VKDREFLECT_HPP
#include "GraphicsVulkan/vkd/vkdReflect.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace gvk
{
	class vkdDevice;

	class vkdShader
	{
		public:
			//	i_pDevice NULL: reflection only (no module). NULL and o_Error
			//	if the SPIR-V can't be read or isn't for i_Stage.
			static std::unique_ptr<vkdShader> Create(vkdDevice* i_pDevice, VkShaderStageFlagBits i_Stage,
													 const void* i_pSpirv, size_t i_Size, const char* i_Entry,
													 const char* i_Name, std::string& o_Error);
			~vkdShader();

			uint64_t					Id() const			{ return m_Id; }
			VkShaderStageFlagBits		Stage() const		{ return m_Stage; }
			VkShaderModule				Module() const		{ return m_Module; }
			const char*					Entry() const		{ return m_Entry.c_str(); }
			const char*					Name() const		{ return m_Name.c_str(); }
			const vkdShaderReflection&	Reflection() const	{ return m_Reflection; }
			//	The SPIR-V words (vkdPipelineCache trims a producer's outputs from them)
			const std::vector<uint32_t>& Spirv() const		{ return m_Spirv; }
			//	Vertex shaders: the inputs a vertex layout must supply
			const std::vector<vkdShaderInput>& Inputs() const { return m_Reflection.m_Inputs; }

		private:
			vkdShader() {}
			vkdShader(const vkdShader&) = delete;
			vkdShader& operator = (const vkdShader&) = delete;

			vkdDevice*			m_pDevice = nullptr;
			uint64_t			m_Id = 0;
			VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_VERTEX_BIT;
			VkShaderModule		m_Module = VK_NULL_HANDLE;
			std::string			m_Entry;
			std::string			m_Name;
			vkdShaderReflection	m_Reflection;
			std::vector<uint32_t> m_Spirv;
	};
}
