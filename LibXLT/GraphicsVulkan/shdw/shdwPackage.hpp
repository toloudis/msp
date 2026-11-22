/*****************************************************************************
**  shdwPackage.hpp
**
**      shdwPackage contains the initialization functions
**	for the shdw package.
**
**	GraphicsVulkan port of GraphicsDX11's shdwPackage: Init installs the
**	shdw renderer creator (shdwSceneRendererCreate); InitGraphics and
**	CleanUpGraphics make and free the render states of the classes
**	ported so far, in DX11's order.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_PACKAGE_HPP
#error shdwPackage.hpp multiply included
#endif
#define GVK_SHDW_PACKAGE_HPP

namespace gvk
{

class shdwPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the shdw package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the shdw package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
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
};

}
