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

#include <string>

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

		//------------------------------------------------------------------------
		//	GetName - the backend's name for people ("DirectX 11");
		//	GetCodeName - its name in command lines, logs and files ("dx11")
		//------------------------------------------------------------------------
		static const char* GetName();
		static const char* GetCodeName();

		//------------------------------------------------------------------------
		//	GetInitError - always empty: Init reports failure by throwing the
		//	g2d exceptions (the other layers' Init returns false instead)
		//------------------------------------------------------------------------
		static const std::string& GetInitError();
};
