#ifdef G3D_RENDERCAMERASPACE_HPP
#error g3dRenderCameraSpace.hpp multiply included
#endif
#define G3D_RENDERCAMERASPACE_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

class g3dRenderCameraSpaceObjects : public g3dRenderLayer
{
public:
	g3dRenderCameraSpaceObjects();
	virtual ~g3dRenderCameraSpaceObjects(void);

	virtual int Render(float i_time);

	static void InitStates();
	static void CleanupStates();
};
