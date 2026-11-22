/*****************************************************************************
**  vkdPipelineCache.cpp
**
**      see vkdPipelineCache.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsVulkan/vkd/vkdPipelineCache.hpp"
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#include "GraphicsVulkan/vkd/private/vkdLinkStages.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <vector>

namespace gvk
{
	namespace
	{
		std::mutex						s_LiveLock;
		std::vector<vkdPipelineCache*>	s_Live;
		std::atomic<bool>				s_bTrimOutputs(true);

		//	FNV-1a over the bytes of one value
		template <class T> void hash(size_t& io_Hash, const T& i_Value)
		{
			const unsigned char* p = reinterpret_cast<const unsigned char*>(&i_Value);
			for (size_t i = 0; i < sizeof(T); i++)
			{
				io_Hash = (io_Hash ^ p[i]) * 1099511628211ull;
			}
		}

		void hash_target(size_t& io_Hash, const vkdBlendTarget& i_Target)
		{
			hash(io_Hash, i_Target.m_bEnable);
			hash(io_Hash, i_Target.m_Src);
			hash(io_Hash, i_Target.m_Dst);
			hash(io_Hash, i_Target.m_Op);
			hash(io_Hash, i_Target.m_SrcAlpha);
			hash(io_Hash, i_Target.m_DstAlpha);
			hash(io_Hash, i_Target.m_OpAlpha);
			hash(io_Hash, i_Target.m_WriteMask);
		}

		bool same_target(const vkdBlendTarget& a, const vkdBlendTarget& b)
		{
			return a.m_bEnable == b.m_bEnable && a.m_Src == b.m_Src && a.m_Dst == b.m_Dst && a.m_Op == b.m_Op &&
				   a.m_SrcAlpha == b.m_SrcAlpha && a.m_DstAlpha == b.m_DstAlpha && a.m_OpAlpha == b.m_OpAlpha &&
				   a.m_WriteMask == b.m_WriteMask;
		}

		//	The blend targets a key uses: all of its colour targets with
		//	independent blending, else only the first (D3D11 uses
		//	RenderTarget[0] for every target then)
		uint32_t blend_targets(const vkdPipelineKey& i_Key)
		{
			return i_Key.m_Blend.m_bIndependent ? i_Key.m_ColorCount : (i_Key.m_ColorCount ? 1 : 0);
		}

		bool same_semantic(const std::string& a, const std::string& b)
		{
			return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y)
			{
				return tolower((unsigned char)x) == tolower((unsigned char)y);
			});
		}

		const VkShaderStageFlagBits c_StageBits[vkdPipelineKey::c_Stages] =
		{
			VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_FRAGMENT_BIT
		};

		const VkPrimitiveTopology c_Topologies[] =
		{
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
		};

		const VkDynamicState c_DynamicStates[] =
		{
			VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT, VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
			VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE,
			VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY, VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE,
			VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE, VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
			VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE, VK_DYNAMIC_STATE_DEPTH_BIAS,
			VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP, VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
			VK_DYNAMIC_STATE_STENCIL_WRITE_MASK, VK_DYNAMIC_STATE_STENCIL_REFERENCE,
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		};
	}

	//------------------------------------------------------------------------
	// vkdPipelineKey
	//------------------------------------------------------------------------
	void vkdPipelineKey::Clear()
	{
		for (uint32_t s = 0; s < c_Stages; s++)
		{
			m_Shaders[s] = 0;
		}
		m_VertexLayout = 0;
		m_TopologyClass = vkd_Triangles;
		m_PatchControlPoints = 0;
		m_ColorCount = 0;
		m_Samples = VK_SAMPLE_COUNT_1_BIT;
		m_Fill = VK_POLYGON_MODE_FILL;
		m_bDepthClip = 1;
		m_LineMode = VK_LINE_RASTERIZATION_MODE_BRESENHAM;
		for (uint32_t c = 0; c < vkdBlendState::c_MaxTargets; c++)
		{
			m_Colors[c] = VK_FORMAT_UNDEFINED;
		}
		m_Depth = VK_FORMAT_UNDEFINED;
		m_Stencil = VK_FORMAT_UNDEFINED;
		m_Blend = vkdBlendState();
	}

	bool vkdPipelineKey::operator == (const vkdPipelineKey& i_Other) const
	{
		for (uint32_t s = 0; s < c_Stages; s++)
		{
			if (m_Shaders[s] != i_Other.m_Shaders[s])
			{
				return false;
			}
		}
		if (m_VertexLayout != i_Other.m_VertexLayout || m_TopologyClass != i_Other.m_TopologyClass ||
			m_PatchControlPoints != i_Other.m_PatchControlPoints || m_ColorCount != i_Other.m_ColorCount ||
			m_Samples != i_Other.m_Samples || m_Fill != i_Other.m_Fill || m_bDepthClip != i_Other.m_bDepthClip ||
			m_LineMode != i_Other.m_LineMode || m_Depth != i_Other.m_Depth ||
			m_Stencil != i_Other.m_Stencil || m_Blend.m_bAlphaToCoverage != i_Other.m_Blend.m_bAlphaToCoverage ||
			m_Blend.m_bIndependent != i_Other.m_Blend.m_bIndependent)
		{
			return false;
		}
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			if (m_Colors[c] != i_Other.m_Colors[c])
			{
				return false;
			}
		}
		for (uint32_t t = 0; t < blend_targets(*this); t++)
		{
			if (!same_target(m_Blend.m_Targets[t], i_Other.m_Blend.m_Targets[t]))
			{
				return false;
			}
		}
		return true;
	}

	size_t vkdPipelineKey::Hash() const
	{
		size_t h = 14695981039346656037ull;
		for (uint32_t s = 0; s < c_Stages; s++)
		{
			hash(h, m_Shaders[s]);
		}
		hash(h, m_VertexLayout);
		hash(h, m_TopologyClass);
		hash(h, m_PatchControlPoints);
		hash(h, m_ColorCount);
		hash(h, m_Samples);
		hash(h, m_Fill);
		hash(h, m_bDepthClip);
		hash(h, m_LineMode);
		hash(h, m_Depth);
		hash(h, m_Stencil);
		hash(h, m_Blend.m_bAlphaToCoverage);
		hash(h, m_Blend.m_bIndependent);
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			hash(h, m_Colors[c]);
		}
		for (uint32_t t = 0; t < blend_targets(*this); t++)
		{
			hash_target(h, m_Blend.m_Targets[t]);
		}
		return h;
	}

	//------------------------------------------------------------------------
	// vkdPipelineCache
	//------------------------------------------------------------------------
	vkdPipelineCache::vkdPipelineCache(vkdDevice& i_Device, vkdBindings& i_Bindings, const std::string& i_File)
		: m_Device(i_Device), m_Bindings(i_Bindings), m_File(i_File)
	{
		load();
		std::lock_guard<std::mutex> lock(s_LiveLock);
		s_Live.push_back(this);
	}

	vkdPipelineCache::~vkdPipelineCache()
	{
		{
			std::lock_guard<std::mutex> lock(s_LiveLock);
			s_Live.erase(std::remove(s_Live.begin(), s_Live.end(), this), s_Live.end());
		}
		if (!m_File.empty())
		{
			Save();
		}
		for (auto& entry : m_Pipelines)
		{
			destroy_later(entry.second.m_Pipeline);
		}
		for (auto& linked : m_Linked)
		{
			destroy_later(linked.second);
		}
		vkDestroyPipelineCache(m_Device.Device(), m_Cache, NULL);
	}

	void vkdPipelineCache::load()
	{
		std::vector<char> data;
		if (!m_File.empty())
		{
			std::ifstream in(std::filesystem::u8path(m_File), std::ios::binary);
			if (in)
			{
				data.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
				const VkPhysicalDeviceProperties& properties = m_Device.Properties();
				VkPipelineCacheHeaderVersionOne header = {};
				std::string reject;
				if (data.size() < sizeof(header))
				{
					reject = "shorter than a cache header";
				}
				else
				{
					memcpy(&header, data.data(), sizeof(header));
					if (header.headerSize < sizeof(header) || header.headerSize > data.size() ||
						header.headerVersion != VK_PIPELINE_CACHE_HEADER_VERSION_ONE)
					{
						reject = "not a Vulkan pipeline cache";
					}
					else if (header.vendorID != properties.vendorID || header.deviceID != properties.deviceID ||
							 memcmp(header.pipelineCacheUUID, properties.pipelineCacheUUID, VK_UUID_SIZE) != 0)
					{
						reject = "made by another GPU or driver";
					}
				}
				if (!reject.empty())
				{
					DBG_LOG("vkdPipelineCache: ignoring " << m_File << " (" << reject << ")");
					data.clear();
				}
			}
		}

		VkPipelineCacheCreateInfo info = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
		info.initialDataSize = data.size();
		info.pInitialData = data.empty() ? NULL : data.data();
		if (vkCreatePipelineCache(m_Device.Device(), &info, NULL, &m_Cache) != VK_SUCCESS && !data.empty())
		{
			DBG_LOG("vkdPipelineCache: the driver rejected " << m_File);
			info.initialDataSize = 0;
			info.pInitialData = NULL;
			data.clear();
			VK_CHECK(vkCreatePipelineCache(m_Device.Device(), &info, NULL, &m_Cache));
		}
		m_bLoaded = !data.empty();
		if (m_bLoaded)
		{
			DBG_LOG("vkdPipelineCache: loaded " << data.size() << " bytes from " << m_File);
		}
	}

	bool vkdPipelineCache::Save()
	{
		if (m_File.empty())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock(m_Lock);
		size_t size = 0;
		if (vkGetPipelineCacheData(m_Device.Device(), m_Cache, &size, NULL) != VK_SUCCESS)
		{
			return false;
		}
		std::vector<char> data(size);
		if (vkGetPipelineCacheData(m_Device.Device(), m_Cache, &size, data.data()) != VK_SUCCESS)
		{
			return false;
		}
		data.resize(size);

		const std::string temp = m_File + ".tmp";
		{
			std::ofstream out(std::filesystem::u8path(temp), std::ios::binary | std::ios::trunc);
			out.write(data.data(), data.size());
			if (!out)
			{
				DBG_LOG("vkdPipelineCache: can't write " << temp);
				return false;
			}
		}
		std::error_code ec;
		std::filesystem::rename(std::filesystem::u8path(temp), std::filesystem::u8path(m_File), ec);
		if (ec)
		{
			DBG_LOG("vkdPipelineCache: can't replace " << m_File << " (" << ec.message() << ")");
			return false;
		}
		return true;
	}

	size_t vkdPipelineCache::Count() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		size_t count = 0;
		for (const auto& entry : m_Pipelines)
		{
			count += entry.second.m_Pipeline ? 1 : 0;
		}
		return count;
	}

	uint64_t vkdPipelineCache::Creations() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_Creations;
	}

	uint64_t vkdPipelineCache::Trimmed() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_Trimmed;
	}

	void vkdPipelineCache::destroy_later(VkPipeline i_Pipeline)
	{
		if (i_Pipeline)
		{
			VkDevice device = m_Device.Device();
			m_Device.DeferDestroy([device, i_Pipeline]()
			{
				vkDestroyPipeline(device, i_Pipeline, NULL);
			});
		}
	}

	void vkdPipelineCache::destroy_later(VkShaderModule i_Module)
	{
		if (i_Module)
		{
			VkDevice device = m_Device.Device();
			m_Device.DeferDestroy([device, i_Module]()
			{
				vkDestroyShaderModule(device, i_Module, NULL);
			});
		}
	}

	void vkdPipelineCache::SetTrimOutputs(bool i_bTrim)
	{
		s_bTrimOutputs = i_bTrim;
	}

	bool vkdPipelineCache::TrimOutputs()
	{
		return s_bTrimOutputs;
	}

	VkShaderModule vkdPipelineCache::linked_module(const vkdShader& i_Producer, const vkdShader& i_Consumer)
	{
		const std::pair<uint64_t, uint64_t> pair(i_Producer.Id(), i_Consumer.Id());
		auto found = m_Linked.find(pair);
		if (found != m_Linked.end())
		{
			return found->second ? found->second : i_Producer.Module();
		}
		VkShaderModule module = VK_NULL_HANDLE;
		const std::vector<uint32_t> trimmed = vkdTrimUnconsumedOutputs(i_Producer.Spirv(), i_Consumer.Spirv());
		if (trimmed != i_Producer.Spirv())
		{
			VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
			info.codeSize = trimmed.size() * sizeof(uint32_t);
			info.pCode = trimmed.data();
			if (VK_CHECK(vkCreateShaderModule(m_Device.Device(), &info, NULL, &module)) != VK_SUCCESS)
			{
				module = VK_NULL_HANDLE;		// use the producer as it is
			}
			else
			{
				const std::string name = std::string(i_Producer.Name()) + " for " + i_Consumer.Name();
				m_Device.SetName(VK_OBJECT_TYPE_SHADER_MODULE, reinterpret_cast<uint64_t>(module), name.c_str());
				m_Trimmed++;
			}
		}
		m_Linked[pair] = module;
		return module ? module : i_Producer.Module();
	}

	void vkdPipelineCache::Evict(uint64_t i_ShaderId)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		for (auto it = m_Pipelines.begin(); it != m_Pipelines.end(); )
		{
			const uint64_t* shaders = it->second.m_Shaders;
			if (std::find(shaders, shaders + vkdPipelineKey::c_Stages, i_ShaderId) != shaders + vkdPipelineKey::c_Stages)
			{
				destroy_later(it->second.m_Pipeline);
				it = m_Pipelines.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (auto it = m_Linked.begin(); it != m_Linked.end(); )
		{
			if (it->first.first == i_ShaderId || it->first.second == i_ShaderId)
			{
				destroy_later(it->second);
				it = m_Linked.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void vkdPipelineCache::EvictEverywhere(uint64_t i_ShaderId)
	{
		std::lock_guard<std::mutex> lock(s_LiveLock);
		for (vkdPipelineCache* cache : s_Live)
		{
			cache->Evict(i_ShaderId);
		}
	}

	VkPipeline vkdPipelineCache::Get(const vkdPipelineKey& i_Key, const vkdShader* const i_Stages[vkdPipelineKey::c_Stages],
									 const vkdVertexLayout* i_pLayout)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		auto found = m_Pipelines.find(i_Key);
		if (found != m_Pipelines.end())
		{
			return found->second.m_Pipeline;
		}
		//	a failure is kept too (as VK_NULL_HANDLE), so it is logged once
		Entry entry;
		entry.m_Pipeline = create(i_Key, i_Stages, i_pLayout);
		for (uint32_t s = 0; s < vkdPipelineKey::c_Stages; s++)
		{
			entry.m_Shaders[s] = i_Key.m_Shaders[s];
		}
		m_Pipelines[i_Key] = entry;
		return entry.m_Pipeline;
	}

	VkPipeline vkdPipelineCache::create(const vkdPipelineKey& i_Key, const vkdShader* const i_Stages[vkdPipelineKey::c_Stages],
										const vkdVertexLayout* i_pLayout)
	{
		std::string name;
		VkPipelineShaderStageCreateInfo stages[vkdPipelineKey::c_Stages] = {};
		uint32_t stageCount = 0;
		const vkdShader* vs = nullptr;
		for (uint32_t s = 0; s < vkdPipelineKey::c_Stages; s++)
		{
			const vkdShader* shader = i_Stages[s];
			if (!shader)
			{
				continue;
			}
			if (shader->Stage() != c_StageBits[s] || shader->Id() != i_Key.m_Shaders[s] || !shader->Module())
			{
				DBG_ASSERT(false, "vkdPipelineCache::Get: the stages don't match the key");
				return VK_NULL_HANDLE;
			}
			VkPipelineShaderStageCreateInfo& stage = stages[stageCount++];
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = &m_Bindings.ShaderMapping();
			stage.stage = shader->Stage();
			stage.module = shader->Module();
			stage.pName = shader->Entry();
			name += (name.empty() ? "" : " + ") + std::string(shader->Name());
			if (s == 0)
			{
				vs = shader;
			}
		}
		if (!vs)
		{
			DBG_LOG("vkdPipelineCache: " << name << ": no vertex shader");
			return VK_NULL_HANDLE;
		}

		//	The last pre-rasterization stage (geometry, else domain, else
		//	vertex shader) linked to the pixel shader: stages[] holds the
		//	present stages in order, so it is the one before the last
		const uint32_t c_PS = vkdPipelineKey::c_Stages - 1;
		if (s_bTrimOutputs && i_Stages[c_PS] && stageCount > 1)
		{
			for (uint32_t s = c_PS; s-- > 0; )
			{
				if (i_Stages[s])
				{
					stages[stageCount - 2].module = linked_module(*i_Stages[s], *i_Stages[c_PS]);
					break;
				}
			}
		}

		//	Vertex inputs, matched to the layout by semantic, as CreateInputLayout does
		std::vector<VkVertexInputAttributeDescription> attributes;
		std::set<uint32_t> slots;
		for (const vkdShaderInput& input : vs->Inputs())
		{
			const vkdVertexElement* element = nullptr;
			if (i_pLayout)
			{
				for (const vkdVertexElement& e : i_pLayout->Elements())
				{
					if (e.m_SemanticIndex == input.m_Index && same_semantic(e.m_Semantic, input.m_Semantic))
					{
						element = &e;
						break;
					}
				}
			}
			if (!element)
			{
				DBG_LOG("vkdPipelineCache: " << name << ": the vertex layout has no " << input.m_Semantic << input.m_Index);
				return VK_NULL_HANDLE;
			}
			attributes.push_back({ input.m_Location, element->m_Slot, element->m_Format, element->m_Offset });
			slots.insert(element->m_Slot);
		}
		std::vector<VkVertexInputBindingDescription> bindings;
		for (uint32_t slot : slots)
		{
			VkVertexInputBindingDescription binding = { slot, i_pLayout->Stride(slot), VK_VERTEX_INPUT_RATE_VERTEX };
			for (const vkdVertexElement& e : i_pLayout->Elements())
			{
				if (e.m_Slot == slot && e.m_bPerInstance)
				{
					binding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
					if (e.m_InstanceStep > 1)
					{
						GVK_NOT_YET("vkdPipelineCache: instance data step rate > 1");
					}
				}
			}
			bindings.push_back(binding);
		}
		VkPipelineVertexInputStateCreateInfo vertex = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
		vertex.vertexBindingDescriptionCount = (uint32_t)bindings.size();
		vertex.pVertexBindingDescriptions = bindings.data();
		vertex.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
		vertex.pVertexAttributeDescriptions = attributes.data();

		VkPipelineInputAssemblyStateCreateInfo assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
		assembly.topology = c_Topologies[i_Key.m_TopologyClass];
		VkPipelineTessellationStateCreateInfo tessellation = {VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
		tessellation.patchControlPoints = i_Key.m_PatchControlPoints;
		VkPipelineViewportStateCreateInfo viewport = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};

		VkPipelineRasterizationStateCreateInfo raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
		raster.depthClampEnable = i_Key.m_bDepthClip ? VK_FALSE : VK_TRUE;
		raster.polygonMode = i_Key.m_Fill;
		//	Lines and wireframe edges as D3D11 draws them: quadrilaterals 1.4
		//	pixels wide when the state has MultisampleEnable, else aliased
		//	(the diamond-exit rule, Vulkan's Bresenham)
		VkPipelineRasterizationLineStateCreateInfo line = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO};
		line.lineRasterizationMode = i_Key.m_LineMode;
		raster.lineWidth = i_Key.m_LineMode == VK_LINE_RASTERIZATION_MODE_RECTANGULAR ? 1.4f : 1.0f;
		raster.pNext = &line;

		VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
		multisample.rasterizationSamples = (VkSampleCountFlagBits)i_Key.m_Samples;
		multisample.alphaToCoverageEnable = i_Key.m_Blend.m_bAlphaToCoverage ? VK_TRUE : VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depth = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

		VkPipelineColorBlendAttachmentState attachments[vkdBlendState::c_MaxTargets] = {};
		for (uint32_t c = 0; c < i_Key.m_ColorCount; c++)
		{
			const vkdBlendTarget& t = i_Key.m_Blend.m_Targets[i_Key.m_Blend.m_bIndependent ? c : 0];
			attachments[c].blendEnable = t.m_bEnable ? VK_TRUE : VK_FALSE;
			attachments[c].srcColorBlendFactor = t.m_Src;
			attachments[c].dstColorBlendFactor = t.m_Dst;
			attachments[c].colorBlendOp = t.m_Op;
			attachments[c].srcAlphaBlendFactor = t.m_SrcAlpha;
			attachments[c].dstAlphaBlendFactor = t.m_DstAlpha;
			attachments[c].alphaBlendOp = t.m_OpAlpha;
			attachments[c].colorWriteMask = t.m_WriteMask;
		}
		VkPipelineColorBlendStateCreateInfo blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
		blend.attachmentCount = i_Key.m_ColorCount;
		blend.pAttachments = attachments;

		VkPipelineDynamicStateCreateInfo dynamic = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
		dynamic.dynamicStateCount = (uint32_t)(sizeof(c_DynamicStates) / sizeof(c_DynamicStates[0]));
		dynamic.pDynamicStates = c_DynamicStates;

		VkPipelineRenderingCreateInfo rendering = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
		rendering.colorAttachmentCount = i_Key.m_ColorCount;
		rendering.pColorAttachmentFormats = i_Key.m_Colors;
		rendering.depthAttachmentFormat = i_Key.m_Depth;
		rendering.stencilAttachmentFormat = i_Key.m_Stencil;
		VkPipelineCreateFlags2CreateInfo flags = {VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO};
		flags.pNext = &rendering;
		flags.flags = VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT;

		VkGraphicsPipelineCreateInfo info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
		info.pNext = &flags;
		info.stageCount = stageCount;
		info.pStages = stages;
		info.pVertexInputState = &vertex;
		info.pInputAssemblyState = &assembly;
		info.pTessellationState = i_Key.m_TopologyClass == vkd_Patches ? &tessellation : NULL;
		info.pViewportState = &viewport;
		info.pRasterizationState = &raster;
		info.pMultisampleState = &multisample;
		info.pDepthStencilState = &depth;
		info.pColorBlendState = &blend;
		info.pDynamicState = &dynamic;
		info.layout = VK_NULL_HANDLE;		// descriptor heaps: no pipeline layout

		VkPipeline pipeline = VK_NULL_HANDLE;
		if (VK_CHECK(vkCreateGraphicsPipelines(m_Device.Device(), m_Cache, 1, &info, NULL, &pipeline)) != VK_SUCCESS)
		{
			DBG_LOG("vkdPipelineCache: " << name << ": vkCreateGraphicsPipelines failed");
			return VK_NULL_HANDLE;
		}
		m_Device.SetName(VK_OBJECT_TYPE_PIPELINE, reinterpret_cast<uint64_t>(pipeline), name.c_str());
		m_Creations++;
		return pipeline;
	}
}
