/****************************************************************************\
**	g3dStateMgr.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dStateMgr (g3dStateMgrDX11):
**	the default rasterizer, depth-stencil and blend states, set on
**	vkdContext.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DSTATEMGR_HPP
#error g3dStateMgr.hpp multiply included
#endif
#define GVK_G3DSTATEMGR_HPP

namespace gvk
{
	namespace g3dStateMgrVk
	{
		enum eCull
		{
			CULL_NONE,
			CULL_FRONT,
			CULL_BACK,
			NUM_CULL_MODES
		};

		enum eFill
		{
			FILL_SOLID,
			FILL_WIREFRAME,
			NUM_FILL_MODES
		};

		enum eDepthEnable
		{
			DEPTH_DISABLE,
			DEPTH_ENABLE,
			NUM_DEPTHENABLE_MODES
		};

		enum eDepthWrite
		{
			DEPTH_WRITE_OFF,
			DEPTH_WRITE_ON,
			NUM_DEPTHWRITE_MODES
		};

		void Init();
		void CleanUp();

		void SetCull(eCull i_Cull);
		void SetFill(eFill i_Fill);

		void SetDefault();
		void RSSetDefault();
		void RSSetWire();
		void BSSetDefault();
		void DSSetDefault();

		void SetDeviceStates();
	}
}
