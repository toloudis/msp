/****************************************************************************\
**  g3dBlendStateMgr.hpp
**
**	The g3dBlendStateMgr sets changes for blendstate.
**
**	GraphicsVulkan port of GraphicsDX11's g3dBlendStateMgr: the same calls,
**	with Vulkan enums for the D3D11 ones. A BlendState is a description
**	(vkdBlendState) that vkdContext turns into pipeline state; nothing is
**	created on the device.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DBLENDSTATEMGR_HPP
#error g3dBlendStateMgr.hpp multiply included
#endif
#define GVK_G3DBLENDSTATEMGR_HPP

#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

namespace gvk
{
	namespace g3dBlendStateMgr
	{
		class BlendState
		{
		public:
			BlendState() {}
			//	Render target 0 only, as in DX11 (no independent blending)
			BlendState( bool i_AlphaToCoverage, bool i_BlendEnabled, VkBlendFactor i_SrcBlend, VkBlendFactor i_DestBlend, VkBlendOp i_BlendOp,
						VkBlendFactor i_SrcBlendAlpha, VkBlendFactor i_DestBlendAlpha, VkBlendOp i_BlendOpAlpha,
						VkColorComponentFlags i_ColorWrittenEnabled );

			bool operator == (const BlendState& i_Item) const;

			vkdBlendState m_Desc;
		};

		void Init();
		void CleanUp();

		void SetDefaultBlendState();
		void SetBlendState( const BlendState* i_pBlendState, const maFloatRGBA& i_BlendFactors = maFloatRGBA() );
		void GetCurrentBlendState( BlendState& o_blendState );
	}
}
