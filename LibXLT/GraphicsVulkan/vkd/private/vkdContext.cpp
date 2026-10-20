/*****************************************************************************
**  vkdContext.cpp
**
**      see vkdContext.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <algorithm>
#include <cstring>
#include <mutex>

namespace gvk
{
	namespace
	{
		vkdTopologyClass topology_class(VkPrimitiveTopology i_Topology)
		{
			switch (i_Topology)
			{
				case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
					return vkd_Points;
				case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
				case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
				case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
				case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
					return vkd_Lines;
				case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST:
					return vkd_Patches;
				default:
					return vkd_Triangles;
			}
		}

		bool is_strip(VkPrimitiveTopology i_Topology)
		{
			return i_Topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP || i_Topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP ||
				   i_Topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY ||
				   i_Topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		}

		bool has_stencil(VkFormat i_Format)
		{
			return i_Format == VK_FORMAT_D24_UNORM_S8_UINT || i_Format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
				   i_Format == VK_FORMAT_D16_UNORM_S8_UINT || i_Format == VK_FORMAT_S8_UINT;
		}

		bool has_depth(VkFormat i_Format)
		{
			return i_Format != VK_FORMAT_S8_UINT;
		}
	}

	vkdContext::vkdContext(vkdDevice& i_Device, vkdRingBuffer& i_Ring, vkdBindings& i_Bindings, vkdPipelineCache& i_Pipelines)
		: m_Device(i_Device), m_Ring(i_Ring), m_Bindings(i_Bindings), m_Pipelines(i_Pipelines)
	{
		ResetState();
		//	Code that records into the device's command buffer, or submits it,
		//	gets it outside our rendering scope
		m_Device.SetRecordingHook([this]()
		{
			if (!m_bRecording)
			{
				if (m_bRendering && m_Commands)
				{
					end_rendering(m_Commands);
				}
				//	what it records is ordered after us, and what we record next after it
				m_bNeedBarrier = true;
			}
		});
	}

	vkdContext::~vkdContext()
	{
		m_Device.SetRecordingHook(std::function<void()>());
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		if (m_bRendering && m_Commands && m_CommandsValue == m_Device.PendingValue())
		{
			end_rendering(m_Commands);
		}
	}

	void vkdContext::ResetState()
	{
		for (uint32_t s = 0; s < vkdPipelineKey::c_Stages; s++)
		{
			m_Shaders[s] = nullptr;
		}
		m_pLayout = nullptr;
		for (uint32_t b = 0; b < c_MaxVertexBuffers; b++)
		{
			m_VertexBuffers[b] = { VK_NULL_HANDLE, 0 };
		}
		m_IndexBuffer = VK_NULL_HANDLE;
		m_IndexOffset = 0;
		m_IndexType = VK_INDEX_TYPE_UINT16;
		m_Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_PatchControlPoints = 0;
		m_Blend = vkdBlendState();
		for (int i = 0; i < 4; i++)
		{
			m_BlendFactor[i] = 1.0f;
		}
		m_DepthStencil = vkdDepthStencilState();
		m_StencilRef = 0;
		m_Rasterizer = vkdRasterizerState();
		m_ViewportCount = 0;
		m_ScissorCount = 0;
		SetRenderTargets(0, nullptr, nullptr);
		m_bUavs = false;
		m_bDynamicDirty = true;
		m_bBuffersDirty = true;
	}

	//------------------------------------------------------------------------
	// State
	//------------------------------------------------------------------------
	void vkdContext::SetShaders(const vkdShader* const i_Stages[vkdPipelineKey::c_Stages])
	{
		for (uint32_t s = 0; s < vkdPipelineKey::c_Stages; s++)
		{
			m_Shaders[s] = i_Stages[s];
		}
	}

	void vkdContext::SetVertexLayout(const vkdVertexLayout* i_pLayout)
	{
		m_pLayout = i_pLayout;
		m_bBuffersDirty = true;
	}

	void vkdContext::SetVertexBuffer(uint32_t i_Slot, VkBuffer i_Buffer, VkDeviceSize i_Offset)
	{
		DBG_ASSERT(i_Slot < c_MaxVertexBuffers, "vkdContext: vertex buffer slot " << i_Slot);
		m_VertexBuffers[i_Slot] = { i_Buffer, i_Offset };
		m_bBuffersDirty = true;
	}

	void vkdContext::SetIndexBuffer(VkBuffer i_Buffer, VkDeviceSize i_Offset, VkIndexType i_Type)
	{
		m_IndexBuffer = i_Buffer;
		m_IndexOffset = i_Offset;
		m_IndexType = i_Type;
		m_bBuffersDirty = true;
	}

	void vkdContext::SetTopology(VkPrimitiveTopology i_Topology, uint32_t i_PatchControlPoints)
	{
		m_Topology = i_Topology;
		m_PatchControlPoints = i_PatchControlPoints;
		m_bDynamicDirty = true;
	}

	void vkdContext::SetBlendState(const vkdBlendState& i_State, const float i_Factor[4])
	{
		m_Blend = i_State;
		for (int i = 0; i < 4; i++)
		{
			m_BlendFactor[i] = i_Factor ? i_Factor[i] : 1.0f;
		}
		m_bDynamicDirty = true;
	}

	void vkdContext::SetDepthStencilState(const vkdDepthStencilState& i_State, uint32_t i_StencilRef)
	{
		m_DepthStencil = i_State;
		m_StencilRef = i_StencilRef;
		m_bDynamicDirty = true;
	}

	void vkdContext::SetRasterizerState(const vkdRasterizerState& i_State)
	{
		m_Rasterizer = i_State;
		m_bDynamicDirty = true;
	}

	void vkdContext::SetViewports(uint32_t i_Count, const vkdViewport* i_pViewports)
	{
		m_ViewportCount = std::min(i_Count, c_MaxViewports);
		for (uint32_t v = 0; v < m_ViewportCount; v++)
		{
			m_Viewports[v] = i_pViewports[v];
		}
		m_bDynamicDirty = true;
	}

	uint32_t vkdContext::GetViewports(vkdViewport* o_pViewports) const
	{
		for (uint32_t v = 0; v < m_ViewportCount; v++)
		{
			o_pViewports[v] = m_Viewports[v];
		}
		return m_ViewportCount;
	}

	void vkdContext::SetScissorRects(uint32_t i_Count, const VkRect2D* i_pRects)
	{
		m_ScissorCount = std::min(i_Count, c_MaxViewports);
		for (uint32_t s = 0; s < m_ScissorCount; s++)
		{
			m_Scissors[s] = i_pRects[s];
		}
		m_bDynamicDirty = true;
	}

	void vkdContext::SetRenderTargets(uint32_t i_Count, vkdImage* const* i_ppColors, vkdImage* i_pDepth)
	{
		DBG_ASSERT(i_Count <= vkdBlendState::c_MaxTargets, "vkdContext: " << i_Count << " render targets");
		bool same = (i_Count == m_ColorCount && i_pDepth == m_pDepth);
		for (uint32_t c = 0; same && c < i_Count; c++)
		{
			same = (m_Colors[c] == i_ppColors[c]);
		}
		if (same)
		{
			return;
		}
		if (m_bRendering)
		{
			std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
			if (m_CommandsValue == m_Device.PendingValue())
			{
				end_rendering(m_Commands);
			}
			m_bRendering = false;
		}
		for (uint32_t c = 0; c < vkdBlendState::c_MaxTargets; c++)
		{
			m_Colors[c] = c < i_Count ? i_ppColors[c] : nullptr;
		}
		m_ColorCount = i_Count;
		m_pDepth = i_pDepth;
	}

	uint32_t vkdContext::GetRenderTargets(vkdImage** o_ppColors, vkdImage** o_ppDepth) const
	{
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			o_ppColors[c] = m_Colors[c];
		}
		if (o_ppDepth)
		{
			*o_ppDepth = m_pDepth;
		}
		return m_ColorCount;
	}

	vkdRingBuffer::Allocation vkdContext::Upload(const void* i_pData, VkDeviceSize i_Size, VkDeviceSize i_Alignment)
	{
		vkdRingBuffer::Allocation allocation = m_Ring.Allocate(i_Size, i_Alignment);
		memcpy(allocation.m_pCpu, i_pData, (size_t)i_Size);
		return allocation;
	}

	//------------------------------------------------------------------------
	// Recording
	//------------------------------------------------------------------------
	//	The open command buffer; a new one gets the heaps bound, all state
	//	set again, and a barrier after the work before it.
	VkCommandBuffer vkdContext::begin_recording()
	{
		m_bRecording = true;
		VkCommandBuffer commands = m_Device.Commands();
		const uint64_t value = m_Device.PendingValue();
		if (commands != m_Commands || value != m_CommandsValue)
		{
			m_Commands = commands;
			m_CommandsValue = value;
			m_bRendering = false;
			m_BoundPipeline = VK_NULL_HANDLE;
			m_bDynamicDirty = true;
			m_bBuffersDirty = true;
			m_bNeedBarrier = true;
			m_Bindings.BindHeaps(commands);
		}
		return commands;
	}

	void vkdContext::barrier_if_needed(VkCommandBuffer i_Commands)
	{
		if (m_bNeedBarrier)
		{
			vkdFullBarrier(i_Commands);
			m_bNeedBarrier = false;
		}
	}

	void vkdContext::end_rendering(VkCommandBuffer i_Commands)
	{
		if (m_bRendering)
		{
			vkCmdEndRendering(i_Commands);
			m_Device.EndLabel(i_Commands);
			m_bRendering = false;
			m_bNeedBarrier = true;
		}
	}

	bool vkdContext::is_bound(const vkdImage* i_pImage) const
	{
		if (i_pImage == m_pDepth)
		{
			return true;
		}
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			if (m_Colors[c] == i_pImage)
			{
				return true;
			}
		}
		return false;
	}

	bool vkdContext::begin_rendering(VkCommandBuffer i_Commands)
	{
		if (m_bRendering)
		{
			return true;
		}
		VkRenderingAttachmentInfo colors[vkdBlendState::c_MaxTargets] = {};
		VkRenderingAttachmentInfo depth = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
		VkRenderingAttachmentInfo stencil = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
		uint32_t width = UINT32_MAX, height = UINT32_MAX;
		const char* label = "vkdContext: no targets";
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			colors[c].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			if (m_Colors[c])
			{
				colors[c].imageView = m_Colors[c]->View();
				colors[c].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				colors[c].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				colors[c].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				width = std::min(width, m_Colors[c]->Width());
				height = std::min(height, m_Colors[c]->Height());
			}
		}
		VkRenderingInfo info = {VK_STRUCTURE_TYPE_RENDERING_INFO};
		if (m_pDepth)
		{
			depth.imageView = m_pDepth->View();
			depth.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			depth.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			stencil = depth;
			info.pDepthAttachment = has_depth(m_pDepth->Format()) ? &depth : NULL;
			info.pStencilAttachment = has_stencil(m_pDepth->Format()) ? &stencil : NULL;
			width = std::min(width, m_pDepth->Width());
			height = std::min(height, m_pDepth->Height());
		}
		if (width == UINT32_MAX)
		{
			//	no attachments (UAV-only drawing): the viewport says how big
			if (m_ViewportCount == 0)
			{
				return false;
			}
			width = (uint32_t)(m_Viewports[0].m_X + m_Viewports[0].m_Width);
			height = (uint32_t)(m_Viewports[0].m_Y + m_Viewports[0].m_Height);
		}
		info.renderArea = { { 0, 0 }, { width, height } };
		info.layerCount = 1;
		info.colorAttachmentCount = m_ColorCount;
		info.pColorAttachments = colors;

		barrier_if_needed(i_Commands);
		m_Device.BeginLabel(i_Commands, label);
		vkCmdBeginRendering(i_Commands, &info);
		m_bRendering = true;
		m_RenderExtent = { width, height };
		m_bDynamicDirty = true;
		return true;
	}

	void vkdContext::set_dynamic_state(VkCommandBuffer i_Commands)
	{
		if (!m_bDynamicDirty)
		{
			return;
		}
		m_bDynamicDirty = false;

		//	Viewports, flipped: D3D's top-left origin and +Y up in clip space
		VkViewport viewports[c_MaxViewports];
		VkRect2D scissors[c_MaxViewports];
		for (uint32_t v = 0; v < m_ViewportCount; v++)
		{
			const vkdViewport& d3d = m_Viewports[v];
			viewports[v] = { d3d.m_X, d3d.m_Y + d3d.m_Height, d3d.m_Width, -d3d.m_Height, d3d.m_MinDepth, d3d.m_MaxDepth };
			if (m_Rasterizer.m_bScissor)
			{
				scissors[v] = v < m_ScissorCount ? m_Scissors[v] : VkRect2D{ { 0, 0 }, { 0, 0 } };
			}
			else
			{
				scissors[v] = { { 0, 0 }, m_RenderExtent };
			}
		}
		vkCmdSetViewportWithCount(i_Commands, m_ViewportCount, viewports);
		vkCmdSetScissorWithCount(i_Commands, m_ViewportCount, scissors);

		vkCmdSetCullMode(i_Commands, m_Rasterizer.m_Cull);
		vkCmdSetFrontFace(i_Commands, m_Rasterizer.m_bFrontCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE);
		const bool bias = m_Rasterizer.m_DepthBias != 0 || m_Rasterizer.m_SlopeScaledDepthBias != 0.0f;
		vkCmdSetDepthBiasEnable(i_Commands, bias ? VK_TRUE : VK_FALSE);
		vkCmdSetDepthBias(i_Commands, (float)m_Rasterizer.m_DepthBias, m_Rasterizer.m_DepthBiasClamp,
						  m_Rasterizer.m_SlopeScaledDepthBias);
		vkCmdSetPrimitiveTopology(i_Commands, m_Topology);
		vkCmdSetPrimitiveRestartEnable(i_Commands, is_strip(m_Topology) ? VK_TRUE : VK_FALSE);

		const vkdDepthStencilState& ds = m_DepthStencil;
		vkCmdSetDepthTestEnable(i_Commands, ds.m_bDepth ? VK_TRUE : VK_FALSE);
		vkCmdSetDepthWriteEnable(i_Commands, ds.m_bDepth && ds.m_bDepthWrite ? VK_TRUE : VK_FALSE);
		vkCmdSetDepthCompareOp(i_Commands, ds.m_DepthFunc);
		vkCmdSetStencilTestEnable(i_Commands, ds.m_bStencil ? VK_TRUE : VK_FALSE);
		vkCmdSetStencilOp(i_Commands, VK_STENCIL_FACE_FRONT_BIT, ds.m_Front.m_Fail, ds.m_Front.m_Pass, ds.m_Front.m_DepthFail, ds.m_Front.m_Func);
		vkCmdSetStencilOp(i_Commands, VK_STENCIL_FACE_BACK_BIT, ds.m_Back.m_Fail, ds.m_Back.m_Pass, ds.m_Back.m_DepthFail, ds.m_Back.m_Func);
		vkCmdSetStencilCompareMask(i_Commands, VK_STENCIL_FACE_FRONT_AND_BACK, ds.m_ReadMask);
		vkCmdSetStencilWriteMask(i_Commands, VK_STENCIL_FACE_FRONT_AND_BACK, ds.m_WriteMask);
		vkCmdSetStencilReference(i_Commands, VK_STENCIL_FACE_FRONT_AND_BACK, m_StencilRef);
		vkCmdSetBlendConstants(i_Commands, m_BlendFactor);
	}

	//	Everything up to the draw call itself; false: the draw is skipped
	bool vkdContext::prepare_draw(VkCommandBuffer i_Commands)
	{
		if (m_ViewportCount == 0)
		{
			return false;		// D3D11 draws nothing without a viewport
		}
		if (!begin_rendering(i_Commands))
		{
			return false;
		}

		vkdPipelineKey key;
		for (uint32_t s = 0; s < vkdPipelineKey::c_Stages; s++)
		{
			key.m_Shaders[s] = m_Shaders[s] ? m_Shaders[s]->Id() : 0;
		}
		key.m_VertexLayout = m_pLayout ? m_pLayout->Id() : 0;
		key.m_TopologyClass = topology_class(m_Topology);
		key.m_PatchControlPoints = (uint8_t)(key.m_TopologyClass == vkd_Patches ? m_PatchControlPoints : 0);
		key.m_ColorCount = (uint8_t)m_ColorCount;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			key.m_Colors[c] = m_Colors[c] ? m_Colors[c]->Format() : VK_FORMAT_UNDEFINED;
			samples = m_Colors[c] ? m_Colors[c]->Samples() : samples;
		}
		if (m_pDepth)
		{
			key.m_Depth = has_depth(m_pDepth->Format()) ? m_pDepth->Format() : VK_FORMAT_UNDEFINED;
			key.m_Stencil = has_stencil(m_pDepth->Format()) ? m_pDepth->Format() : VK_FORMAT_UNDEFINED;
			samples = m_pDepth->Samples();
		}
		key.m_Samples = (uint8_t)samples;
		key.m_Fill = m_Rasterizer.m_Fill;
		key.m_bDepthClip = m_Rasterizer.m_bDepthClip ? 1 : 0;
		//	D3D11 draws lines as 1.4-pixel quadrilaterals when MultisampleEnable
		//	is set (on any target), aliased otherwise; alpha-antialiased lines
		//	(AntialiasedLineEnable alone) aren't emulated: nothing asks for them
		key.m_LineMode = m_Rasterizer.m_bMultisample ? VK_LINE_RASTERIZATION_MODE_RECTANGULAR : VK_LINE_RASTERIZATION_MODE_BRESENHAM;
		key.m_Blend = m_Blend;

		VkPipeline pipeline = m_Pipelines.Get(key, m_Shaders, m_pLayout);
		if (!pipeline)
		{
			return false;
		}
		if (pipeline != m_BoundPipeline)
		{
			vkCmdBindPipeline(i_Commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			m_BoundPipeline = pipeline;
		}
		set_dynamic_state(i_Commands);

		if (m_bBuffersDirty)
		{
			m_bBuffersDirty = false;
			if (m_pLayout)
			{
				for (uint32_t slot = 0; slot < (uint32_t)m_pLayout->Strides().size() && slot < c_MaxVertexBuffers; slot++)
				{
					if (m_VertexBuffers[slot].m_Buffer)
					{
						vkCmdBindVertexBuffers(i_Commands, slot, 1, &m_VertexBuffers[slot].m_Buffer, &m_VertexBuffers[slot].m_Offset);
					}
				}
			}
			if (m_IndexBuffer)
			{
				vkCmdBindIndexBuffer(i_Commands, m_IndexBuffer, m_IndexOffset, m_IndexType);
			}
		}
		m_Bindings.Apply(i_Commands, m_Ring);
		return true;
	}

	void vkdContext::Draw(uint32_t i_Vertices, uint32_t i_First)
	{
		DrawInstanced(i_Vertices, 1, i_First, 0);
	}

	void vkdContext::DrawIndexed(uint32_t i_Indices, uint32_t i_FirstIndex, int32_t i_BaseVertex)
	{
		DrawIndexedInstanced(i_Indices, 1, i_FirstIndex, i_BaseVertex, 0);
	}

	void vkdContext::DrawInstanced(uint32_t i_Vertices, uint32_t i_Instances, uint32_t i_First, uint32_t i_FirstInstance)
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		if (prepare_draw(commands))
		{
			vkCmdDraw(commands, i_Vertices, i_Instances, i_First, i_FirstInstance);
			G3D_PROFILE_DRAWS(1);
			m_bNeedBarrier |= m_bUavs;
		}
		m_bRecording = false;
	}

	void vkdContext::DrawIndexedInstanced(uint32_t i_Indices, uint32_t i_Instances, uint32_t i_FirstIndex,
										  int32_t i_BaseVertex, uint32_t i_FirstInstance)
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		if (prepare_draw(commands))
		{
			vkCmdDrawIndexed(commands, i_Indices, i_Instances, i_FirstIndex, i_BaseVertex, i_FirstInstance);
			G3D_PROFILE_DRAWS(1);
			m_bNeedBarrier |= m_bUavs;
		}
		m_bRecording = false;
	}

	//------------------------------------------------------------------------
	// Clears
	//------------------------------------------------------------------------
	void vkdContext::ClearRenderTarget(vkdImage* i_pTarget, const float i_Rgba[4])
	{
		if (!i_pTarget)
		{
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		VkClearColorValue color;
		memcpy(color.float32, i_Rgba, sizeof(color.float32));
		uint32_t attachment = UINT32_MAX;
		for (uint32_t c = 0; c < m_ColorCount; c++)
		{
			if (m_Colors[c] == i_pTarget)
			{
				attachment = c;
			}
		}
		if (attachment != UINT32_MAX && begin_rendering(commands))
		{
			VkClearAttachment clear = {};
			clear.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			clear.colorAttachment = attachment;
			clear.clearValue.color = color;
			VkClearRect rect = { { { 0, 0 }, m_RenderExtent }, 0, 1 };
			vkCmdClearAttachments(commands, 1, &clear, 1, &rect);
		}
		else
		{
			DBG_ASSERT(i_pTarget->Usage() & VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					   "vkdContext::ClearRenderTarget: the image needs TRANSFER_DST usage to be cleared outside rendering");
			end_rendering(commands);
			barrier_if_needed(commands);
			//	the part of the image the target views (one face or mip of a texture)
			VkImageSubresourceRange range = i_pTarget->Range();
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			vkCmdClearColorImage(commands, i_pTarget->Image(), VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
			m_bNeedBarrier = true;
		}
		m_bRecording = false;
	}

	void vkdContext::ClearDepthStencil(vkdImage* i_pTarget, bool i_bDepth, float i_Depth, bool i_bStencil, uint8_t i_Stencil)
	{
		if (!i_pTarget || (!i_bDepth && !i_bStencil))
		{
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		VkImageAspectFlags aspects = 0;
		if (i_bDepth && has_depth(i_pTarget->Format()))
		{
			aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		if (i_bStencil && has_stencil(i_pTarget->Format()))
		{
			aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		VkClearDepthStencilValue value = { i_Depth, i_Stencil };
		if (aspects && i_pTarget == m_pDepth && begin_rendering(commands))
		{
			VkClearAttachment clear = {};
			clear.aspectMask = aspects;
			clear.clearValue.depthStencil = value;
			VkClearRect rect = { { { 0, 0 }, m_RenderExtent }, 0, 1 };
			vkCmdClearAttachments(commands, 1, &clear, 1, &rect);
		}
		else if (aspects)
		{
			DBG_ASSERT(i_pTarget->Usage() & VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					   "vkdContext::ClearDepthStencil: the image needs TRANSFER_DST usage to be cleared outside rendering");
			end_rendering(commands);
			barrier_if_needed(commands);
			VkImageSubresourceRange range = i_pTarget->Range();
			range.aspectMask = aspects;
			vkCmdClearDepthStencilImage(commands, i_pTarget->Image(), VK_IMAGE_LAYOUT_GENERAL, &value, 1, &range);
			m_bNeedBarrier = true;
		}
		m_bRecording = false;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	VkCommandBuffer vkdContext::Commands()
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		end_rendering(commands);
		barrier_if_needed(commands);
		m_bRecording = false;
		//	what the caller records is ordered before what we record next
		m_bNeedBarrier = true;
		return commands;
	}

	vkdImage* vkdContext::NullTexture()
	{
		if (!m_pNullTexture)
		{
			m_pNullTexture = vkdImage::Create2D(m_Device, 1, 1, VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "vkdContext null texture");
			const float zero[4] = { 0, 0, 0, 0 };
			ClearRenderTarget(m_pNullTexture.get(), zero);
		}
		return m_pNullTexture.get();
	}

	void vkdContext::SetShaderResource(uint32_t i_Slot, vkdImage* i_pImage)
	{
		vkdImage* image = i_pImage ? i_pImage : NullTexture();
		m_Bindings.SetTexture(i_Slot, image->SampledIndex(m_Bindings));
	}

	void vkdContext::SetShaderResources(uint32_t i_Start, uint32_t i_Count, vkdImage* const* i_ppImages)
	{
		for (uint32_t i = 0; i < i_Count; i++)
			SetShaderResource(i_Start + i, i_ppImages[i]);
	}

	uint64_t vkdContext::Flush()
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = begin_recording();
		end_rendering(commands);
		m_bRecording = false;
		return m_Device.Submit();
	}
}
