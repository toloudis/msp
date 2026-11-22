/*****************************************************************************
**  vkdPipelineCache.hpp
**
**      vkdPipelineCache turns the state DX11 code sets piece by piece into
**	Vulkan graphics pipelines, created on first use and kept:
**
**	- vkdPipelineKey holds what a pipeline bakes in: the shaders, the
**	  vertex layout, the topology class, blend state, fill mode, depth
**	  clip, render target formats and sample count. Everything else
**	  (viewports, scissors, cull mode, front face, topology, depth and
**	  stencil tests, depth bias, blend constants) is dynamic state that
**	  vkdContext sets per draw, so changing it never needs a new pipeline.
**	- Pipelines use VK_EXT_descriptor_heap (no layout; vkdBindings'
**	  shader mapping on every stage) and dynamic rendering.
**	- Vertex inputs are matched to the layout by semantic and index, as
**	  D3D11's CreateInputLayout does; an input the layout doesn't supply
**	  fails the pipeline (logged), as it fails in D3D11.
**	- The stage that feeds the pixel shader (geometry, else domain, else
**	  vertex shader) is linked to it as D3D11 does: a copy of it, made
**	  once per pair of shaders, has no outputs at locations the pixel
**	  shader doesn't declare (vkdTrimUnconsumedOutputs, which explains
**	  why and what changes). The image is the same; the validation layer
**	  just has no unconsumed outputs to warn about. SetTrimOutputs(false)
**	  turns this off (tests compare the two).
**	- The driver's pipeline cache data is saved to a file and loaded at
**	  start, so pipelines from earlier runs are quick to create. A file
**	  from another GPU or driver, or a damaged one, is ignored (logged).
**
**	Phase 1 looks pipelines up at draw time; the key is complete, so
**	Phase 2 can look them up once per object without changing callers.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDPIPELINECACHE_HPP
#error vkdPipelineCache.hpp multiply included
#endif
#define GVK_VKDPIPELINECACHE_HPP

#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace gvk
{
	class vkdBindings;
	class vkdDevice;
	class vkdShader;

	enum vkdTopologyClass : uint8_t { vkd_Points, vkd_Lines, vkd_Triangles, vkd_Patches };

	//	Compared and hashed field by field. Clear() resets it to "no shaders,
	//	no layout, triangles, no targets, D3D11 default blend".
	struct vkdPipelineKey
	{
		static const uint32_t c_Stages = 5;		// VS HS DS GS PS

		uint64_t		m_Shaders[c_Stages];	// vkdShader ids (0 = none)
		uint64_t		m_VertexLayout;			// vkdVertexLayout id (0 = none)
		vkdTopologyClass m_TopologyClass;
		uint8_t			m_PatchControlPoints;
		uint8_t			m_ColorCount;
		uint8_t			m_Samples;				// VkSampleCountFlagBits
		VkPolygonMode	m_Fill;
		uint32_t		m_bDepthClip;
		VkLineRasterizationMode m_LineMode;	// D3D11's line style (vkdContext picks it)
		VkFormat		m_Colors[vkdBlendState::c_MaxTargets];
		VkFormat		m_Depth;				// VK_FORMAT_UNDEFINED if none
		VkFormat		m_Stencil;
		vkdBlendState	m_Blend;

		vkdPipelineKey()						{ Clear(); }
		void Clear();
		bool operator == (const vkdPipelineKey& i_Other) const;
		size_t Hash() const;
	};

	class vkdPipelineCache
	{
		public:
			//	i_File: where the driver's cache data persists (UTF-8; "" = nowhere)
			vkdPipelineCache(vkdDevice& i_Device, vkdBindings& i_Bindings, const std::string& i_File);
			//	Saves (if there is a file), then destroys the pipelines once the GPU is done.
			~vkdPipelineCache();

			//	The pipeline for i_Key, created on first use. i_Stages are the
			//	shaders the key's ids name (VS HS DS GS PS, NULL = none).
			//	VK_NULL_HANDLE (logged once) if it can't be created.
			VkPipeline Get(const vkdPipelineKey& i_Key, const vkdShader* const i_Stages[vkdPipelineKey::c_Stages],
						   const vkdVertexLayout* i_pLayout);

			//	Drops every pipeline that uses the shader (vkdShader's destructor calls this).
			void Evict(uint64_t i_ShaderId);
			//	Evict on every live cache
			static void EvictEverywhere(uint64_t i_ShaderId);

			//	Writes the driver's cache data to the file (via a .tmp file). false if it can't.
			bool Save();

			//	Whether new pipelines link their stages (default true)
			static void	SetTrimOutputs(bool i_bTrim);
			static bool	TrimOutputs();

			size_t		Count() const;
			uint64_t	Creations() const;
			//	Producers made with outputs removed (one per pair of shaders)
			uint64_t	Trimmed() const;
			//	false when there was no file, or it was rejected
			bool		LoadedFromFile() const		{ return m_bLoaded; }

		private:
			struct KeyHash { size_t operator () (const vkdPipelineKey& i_Key) const { return i_Key.Hash(); } };
			struct Entry { VkPipeline m_Pipeline; uint64_t m_Shaders[vkdPipelineKey::c_Stages]; };

			VkPipeline create(const vkdPipelineKey& i_Key, const vkdShader* const i_Stages[vkdPipelineKey::c_Stages],
							  const vkdVertexLayout* i_pLayout);
			void load();
			void destroy_later(VkPipeline i_Pipeline);
			void destroy_later(VkShaderModule i_Module);
			//	i_Producer's module for a pipeline with i_Consumer as pixel shader
			VkShaderModule linked_module(const vkdShader& i_Producer, const vkdShader& i_Consumer);

			vkdDevice&		m_Device;
			vkdBindings&	m_Bindings;
			std::string		m_File;
			VkPipelineCache	m_Cache = VK_NULL_HANDLE;
			bool			m_bLoaded = false;
			uint64_t		m_Creations = 0;
			uint64_t		m_Trimmed = 0;
			std::unordered_map<vkdPipelineKey, Entry, KeyHash> m_Pipelines;
			//	Trimmed producers by (producer id, consumer id); VK_NULL_HANDLE
			//	when the producer is used as it is
			struct PairHash { size_t operator () (const std::pair<uint64_t, uint64_t>& i_Pair) const
							  { return std::hash<uint64_t>()(i_Pair.first * 1099511628211ull ^ i_Pair.second); } };
			std::unordered_map<std::pair<uint64_t, uint64_t>, VkShaderModule, PairHash> m_Linked;
			mutable std::mutex m_Lock;
	};
}
