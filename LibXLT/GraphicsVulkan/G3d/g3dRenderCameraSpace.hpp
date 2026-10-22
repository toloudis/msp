/****************************************************************************\
**  g3dRenderCameraSpace.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dRenderCameraSpaceObjects: a
**	camera-space layer (MachStudio's camera layer, a viewer's overlay
**	layers) drawn by the simple renderer, sorted, over the scene.
\****************************************************************************/
#ifdef GVK_G3D_RENDERCAMERASPACE_HPP
#error g3dRenderCameraSpace.hpp multiply included
#endif
#define GVK_G3D_RENDERCAMERASPACE_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

namespace gvk
{

class g3dRenderCameraSpaceObjects : public g3dRenderLayer
{
public:
	g3dRenderCameraSpaceObjects();
	virtual ~g3dRenderCameraSpaceObjects(void);

	virtual int Render(float i_time);

	static void InitStates();
	static void CleanupStates();
};

}
