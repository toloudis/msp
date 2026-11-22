/*****************************************************************************
**  vkdState.hpp
**
**      The fixed-function state GraphicsDX11's code sets through
**	ID3D11BlendState, ID3D11DepthStencilState, ID3D11RasterizerState,
**	viewports and input layouts, described in Vulkan terms. A default-
**	constructed description is D3D11's default state (what
**	ID3D11DeviceContext::ClearState sets), so ported code that relies on
**	defaults behaves the same.
**
**	vkdContext takes these; vkdPipelineCache bakes the parts Vulkan can't
**	change dynamically into pipelines (blend, fill mode, depth clip), the
**	rest is dynamic state.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDSTATE_HPP
#error vkdState.hpp multiply included
#endif
#define GVK_VKDSTATE_HPP

#include <volk.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gvk
{
	//	D3D11_RENDER_TARGET_BLEND_DESC
	struct vkdBlendTarget
	{
		bool					m_bEnable = false;
		VkBlendFactor			m_Src = VK_BLEND_FACTOR_ONE;
		VkBlendFactor			m_Dst = VK_BLEND_FACTOR_ZERO;
		VkBlendOp				m_Op = VK_BLEND_OP_ADD;
		VkBlendFactor			m_SrcAlpha = VK_BLEND_FACTOR_ONE;
		VkBlendFactor			m_DstAlpha = VK_BLEND_FACTOR_ZERO;
		VkBlendOp				m_OpAlpha = VK_BLEND_OP_ADD;
		VkColorComponentFlags	m_WriteMask = 0xF;
	};

	//	D3D11_BLEND_DESC
	struct vkdBlendState
	{
		static const uint32_t	c_MaxTargets = 8;
		bool					m_bAlphaToCoverage = false;
		bool					m_bIndependent = false;		// false: every target uses m_Targets[0]
		vkdBlendTarget			m_Targets[c_MaxTargets];
	};

	//	D3D11_DEPTH_STENCILOP_DESC
	struct vkdStencilFace
	{
		VkStencilOp	m_Fail = VK_STENCIL_OP_KEEP;
		VkStencilOp	m_DepthFail = VK_STENCIL_OP_KEEP;
		VkStencilOp	m_Pass = VK_STENCIL_OP_KEEP;
		VkCompareOp	m_Func = VK_COMPARE_OP_ALWAYS;
	};

	//	D3D11_DEPTH_STENCIL_DESC
	struct vkdDepthStencilState
	{
		bool			m_bDepth = true;
		bool			m_bDepthWrite = true;
		VkCompareOp		m_DepthFunc = VK_COMPARE_OP_LESS;
		bool			m_bStencil = false;
		uint8_t			m_ReadMask = 0xFF;
		uint8_t			m_WriteMask = 0xFF;
		vkdStencilFace	m_Front;
		vkdStencilFace	m_Back;
	};

	//	D3D11_RASTERIZER_DESC
	struct vkdRasterizerState
	{
		VkPolygonMode	m_Fill = VK_POLYGON_MODE_FILL;
		VkCullModeFlags	m_Cull = VK_CULL_MODE_BACK_BIT;
		bool			m_bFrontCCW = false;
		int32_t			m_DepthBias = 0;
		float			m_DepthBiasClamp = 0.0f;
		float			m_SlopeScaledDepthBias = 0.0f;
		bool			m_bDepthClip = true;
		bool			m_bScissor = false;
		bool			m_bMultisample = false;
		bool			m_bAntialiasedLine = false;
	};

	//	D3D11_VIEWPORT: top-left origin, as D3D gives it (vkdContext flips it)
	struct vkdViewport
	{
		float	m_X = 0.0f;
		float	m_Y = 0.0f;
		float	m_Width = 0.0f;
		float	m_Height = 0.0f;
		float	m_MinDepth = 0.0f;
		float	m_MaxDepth = 1.0f;
	};

	//	D3D11_INPUT_ELEMENT_DESC
	struct vkdVertexElement
	{
		std::string	m_Semantic;				// matched case-insensitively, as D3D does
		uint32_t	m_SemanticIndex = 0;
		VkFormat	m_Format = VK_FORMAT_UNDEFINED;
		uint32_t	m_Slot = 0;				// vertex buffer slot
		uint32_t	m_Offset = 0;			// bytes into the vertex
		bool		m_bPerInstance = false;
		uint32_t	m_InstanceStep = 0;		// D3D's InstanceDataStepRate (1 = every instance)
	};

	//	An input layout: its elements and the stride of each vertex buffer
	//	slot it uses (D3D gives strides with the buffers; Vulkan bakes them
	//	into the pipeline).
	class vkdVertexLayout
	{
		public:
			vkdVertexLayout(std::vector<vkdVertexElement> i_Elements, std::vector<uint32_t> i_Strides);

			uint64_t Id() const									{ return m_Id; }
			const std::vector<vkdVertexElement>& Elements() const	{ return m_Elements; }
			uint32_t Stride(uint32_t i_Slot) const;
			const std::vector<uint32_t>& Strides() const			{ return m_Strides; }

		private:
			uint64_t						m_Id;
			std::vector<vkdVertexElement>	m_Elements;
			std::vector<uint32_t>			m_Strides;		// by slot
	};
}
