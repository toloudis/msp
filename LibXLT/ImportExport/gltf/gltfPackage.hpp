/*****************************************************************************
**  gltfPackage.hpp
**
**      gltfPackage contains the initialization functions
**	for the GLTF importer package.
**
**	StudioGPU
**	Copyright(C) 2024 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef GLTF_PACKAGE_HPP
#error gltfPackage.hpp multiply included
#endif
#define GLTF_PACKAGE_HPP

class gltfPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
