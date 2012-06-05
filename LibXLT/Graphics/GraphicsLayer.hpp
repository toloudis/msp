/*****************************************************************************
**  GraphicsLayer.hpp
**
**      GraphicsLayer contains the initialization functions
**	for the all packages within the Graphics Layer.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GRAPHICS_LAYER_HPP
#error GraphicsLayer.hpp multiply included
#endif
#define GRAPHICS_LAYER_HPP

class g2dSystem;
class g3dSystem;

class GraphicsLayer
{
	public:

		//------------------------------------------------------------------------
		//	Init
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics(g2dSystem* i_pSystem2d, g3dSystem* i_pSystem3d);

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();

		//------------------------------------------------------------------------
		//	Implementation specific system accessors
		//------------------------------------------------------------------------
		static g2dSystem* GetSystem2D();
		static g3dSystem* GetSystem3D();
};
