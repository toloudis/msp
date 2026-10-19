/*****************************************************************************
**  vkdContext.hpp
**
**      vkdContext plays the part of DX11's immediate context for ported
**	code: it holds the current shaders, vertex layout and buffers,
**	topology, blend / depth-stencil / rasterizer state, viewports,
**	scissors, render targets and register-slot bindings, and turns each
**	draw into Vulkan:
**
**	- Rendering (dynamic rendering, attachments in GENERAL layout, load and
**	  store) begins at the first draw or clear after SetRenderTargets and
**	  ends when the targets change, when other code records commands
**	  (vkdDevice's recording hook), or at Flush().
**	- The pipeline comes from vkdPipelineCache by the full state; viewport,
**	  scissor, cull mode, front face, topology, depth/stencil and depth
**	  bias are dynamic state, set when they change.
**	- Viewports are given as D3D gives them (top-left origin) and flipped
**	  with a negative height, which also makes D3D's winding and
**	  FrontCounterClockwise mean the same here.
**	- D3D11 orders every command after the ones before it. Here one global
**	  memory barrier (all earlier writes before all later access) goes
**	  between rendering scopes, around clears and copies outside
**	  rendering, after draws with UAVs, and at the start of each command
**	  buffer. Within one rendering scope, draws to the same attachments are
**	  ordered by rasterization order, as in D3D11. Synchronization
**	  validation checks this; Phase 2 can narrow it.
**	- The initial state, and ResetState(), is D3D11's ClearState().
**
**	One context, used by one thread at a time (the render thread, as
**	DX11's immediate context); it holds the device's stream lock while it
**	records.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDCONTEXT_HPP
#error vkdContext.hpp multiply included
#endif
#define GVK_VKDCONTEXT_HPP

#ifndef GVK_VKDPIPELINECACHE_HPP
#include "GraphicsVulkan/vkd/vkdPipelineCache.hpp"
#endif
#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif
#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>

namespace gvk
{
	class vkdBindings;
	class vkdDevice;
	class vkdImage;
	class vkdShader;

	class vkdContext
	{
		public:
			static const uint32_t c_MaxViewports = 16;
			static const uint32_t c_MaxVertexBuffers = 16;

			vkdContext(vkdDevice& i_Device, vkdRingBuffer& i_Ring, vkdBindings& i_Bindings, vkdPipelineCache& i_Pipelines);
			~vkdContext();

			//	Shaders: VS HS DS GS PS (NULL = stage unused)
			void SetShaders(const vkdShader* const i_Stages[vkdPipelineKey::c_Stages]);
			void SetVertexLayout(const vkdVertexLayout* i_pLayout);
			void SetVertexBuffer(uint32_t i_Slot, VkBuffer i_Buffer, VkDeviceSize i_Offset);
			void SetIndexBuffer(VkBuffer i_Buffer, VkDeviceSize i_Offset, VkIndexType i_Type);
			void SetTopology(VkPrimitiveTopology i_Topology, uint32_t i_PatchControlPoints = 0);

			void SetBlendState(const vkdBlendState& i_State, const float i_Factor[4]);
			void SetDepthStencilState(const vkdDepthStencilState& i_State, uint32_t i_StencilRef);
			void SetRasterizerState(const vkdRasterizerState& i_State);
			const vkdBlendState&		GetBlendState() const			{ return m_Blend; }
			const vkdDepthStencilState&	GetDepthStencilState() const	{ return m_DepthStencil; }
			uint32_t					GetStencilRef() const			{ return m_StencilRef; }
			const vkdRasterizerState&	GetRasterizerState() const		{ return m_Rasterizer; }

			void SetViewports(uint32_t i_Count, const vkdViewport* i_pViewports);
			//	Copies up to c_MaxViewports into o_pViewports; returns how many are set
			uint32_t GetViewports(vkdViewport* o_pViewports) const;
			//	Used only while the rasterizer state enables the scissor test
			void SetScissorRects(uint32_t i_Count, const VkRect2D* i_pRects);

			//	Colour targets and a depth(-stencil) target; all NULL = none
			void SetRenderTargets(uint32_t i_Count, vkdImage* const* i_ppColors, vkdImage* i_pDepth);
			uint32_t GetRenderTargets(vkdImage** o_ppColors /* 8 */, vkdImage** o_ppDepth) const;
			//	Needs TRANSFER_DST usage unless the target is bound and rendering
			void ClearRenderTarget(vkdImage* i_pTarget, const float i_Rgba[4]);
			void ClearDepthStencil(vkdImage* i_pTarget, bool i_bDepth, float i_Depth, bool i_bStencil, uint8_t i_Stencil);

			//	b/t/s/u register slots for the next draws
			vkdBindings& Bindings()		{ return m_Bindings; }
			//	D3D11's *SSetShaderResources(i_Slot, 1, &view): binds i_pImage as
			//	a sampled image at t register i_Slot; NULL binds NullTexture(),
			//	as D3D11 reads a NULL view as zeros
			void SetShaderResource(uint32_t i_Slot, vkdImage* i_pImage);
			//	Short-lived GPU memory (constants, dynamic vertices); valid for work recorded from now on
			vkdRingBuffer::Allocation Upload(const void* i_pData, VkDeviceSize i_Size, VkDeviceSize i_Alignment = 256);
			//	The same, for the caller to fill
			vkdRingBuffer::Allocation Allocate(VkDeviceSize i_Size, VkDeviceSize i_Alignment = 256)	{ return m_Ring.Allocate(i_Size, i_Alignment); }

			void Draw(uint32_t i_Vertices, uint32_t i_First);
			void DrawIndexed(uint32_t i_Indices, uint32_t i_FirstIndex, int32_t i_BaseVertex);
			void DrawInstanced(uint32_t i_Vertices, uint32_t i_Instances, uint32_t i_First, uint32_t i_FirstInstance);
			void DrawIndexedInstanced(uint32_t i_Indices, uint32_t i_Instances, uint32_t i_FirstIndex,
									  int32_t i_BaseVertex, uint32_t i_FirstInstance);
			//	The draws that follow write UAVs: a barrier follows them
			void SetWritesUavs(bool i_bWrites)		{ m_bUavs = i_bWrites; }

			//	For code that records its own commands: ends rendering and
			//	places the barrier earlier work needs. Hold the device's stream lock.
			VkCommandBuffer Commands();
			//	Ends rendering and submits; returns the submission's timeline value
			uint64_t Flush();

			//	D3D11's ClearState(): every state above back to its default
			void ResetState();

			vkdPipelineCache& Pipelines()	{ return m_Pipelines; }

			//	A 1x1 transparent black image (sampled and storage) to bind where
			//	DX11 code binds a NULL view, which reads as zeros in D3D11
			vkdImage* NullTexture();

		private:
			vkdContext(const vkdContext&) = delete;
			vkdContext& operator = (const vkdContext&) = delete;

			VkCommandBuffer begin_recording();
			void end_rendering(VkCommandBuffer i_Commands);
			void barrier_if_needed(VkCommandBuffer i_Commands);
			bool begin_rendering(VkCommandBuffer i_Commands);
			bool prepare_draw(VkCommandBuffer i_Commands);
			void set_dynamic_state(VkCommandBuffer i_Commands);
			bool is_bound(const vkdImage* i_pImage) const;

			vkdDevice&			m_Device;
			vkdRingBuffer&		m_Ring;
			vkdBindings&		m_Bindings;
			vkdPipelineCache&	m_Pipelines;

			//	State, as ported code set it
			const vkdShader*		m_Shaders[vkdPipelineKey::c_Stages];
			const vkdVertexLayout*	m_pLayout;
			struct VertexBuffer { VkBuffer m_Buffer; VkDeviceSize m_Offset; };
			VertexBuffer			m_VertexBuffers[c_MaxVertexBuffers];
			VkBuffer				m_IndexBuffer;
			VkDeviceSize			m_IndexOffset;
			VkIndexType				m_IndexType;
			VkPrimitiveTopology		m_Topology;
			uint32_t				m_PatchControlPoints;
			vkdBlendState			m_Blend;
			float					m_BlendFactor[4];
			vkdDepthStencilState	m_DepthStencil;
			uint32_t				m_StencilRef;
			vkdRasterizerState		m_Rasterizer;
			vkdViewport				m_Viewports[c_MaxViewports];
			uint32_t				m_ViewportCount;
			VkRect2D				m_Scissors[c_MaxViewports];
			uint32_t				m_ScissorCount;
			vkdImage*				m_Colors[vkdBlendState::c_MaxTargets] = {};
			uint32_t				m_ColorCount = 0;
			vkdImage*				m_pDepth = nullptr;
			bool					m_bUavs;
			std::unique_ptr<vkdImage> m_pNullTexture;

			//	Recording
			VkCommandBuffer			m_Commands = VK_NULL_HANDLE;	// the command buffer last recorded into
			uint64_t				m_CommandsValue = 0;			// its timeline value
			bool					m_bRendering = false;
			VkExtent2D				m_RenderExtent = {};
			bool					m_bNeedBarrier = false;
			bool					m_bRecording = false;			// inside one of our own calls
			VkPipeline				m_BoundPipeline = VK_NULL_HANDLE;
			bool					m_bDynamicDirty = true;
			bool					m_bBuffersDirty = true;
	};
}
