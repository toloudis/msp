/*****************************************************************************
**  tmeshPackage.hpp
**
**	GraphicsVulkan port of GraphicsDX11/tmesh/tmeshPackage.hpp (namespace gvk).
**
**      tmeshPackage contains the initialization functions
**	for the tmesh package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_TMESH_PACKAGE_HPP
#error tmeshPackage.hpp multiply included
#endif
#define GVK_TMESH_PACKAGE_HPP

namespace gvk
{

class tmeshPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the tmesh package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the tmesh package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
}
