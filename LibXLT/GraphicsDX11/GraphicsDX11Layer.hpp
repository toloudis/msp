/*****************************************************************************
**  GraphicsDX11Layer.hpp
**
**      GraphicsDX11Layer contains the initialization functions
**	for the all packages within the GraphicsDX11 Layer.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GRAPHICSDX11_LAYER_HPP
#error GraphicsDX11Layer.hpp multiply included
#endif
#define GRAPHICSDX11_LAYER_HPP

class g2dSystem;
class g3dSystem;

class GraphicsDX11Layer
{
	public:

		//------------------------------------------------------------------------
		//	Init
		//------------------------------------------------------------------------
		static void Init(void* i_hWnd);

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();

		//------------------------------------------------------------------------
		//	Accessors for the major graphics system objects
		//------------------------------------------------------------------------
		static g2dSystem* GetSystem2D();
		static g3dSystem* GetSystem3D();
};
