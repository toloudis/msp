/****************************************************************************\
**	mdlModelingPackageListMgr.hpp
**
**		Maintainer of valid modeling packages.
**
**	An application can set the list of valid modeling packages that models
**	and data can be loaded from.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MODELINGPACKAGELIST_HPP
#error mdlModelingPackageListMgr.hpp multiply included
#endif
#define MDL_MODELINGPACKAGELIST_HPP


//============================================================================
//	Forward References
//============================================================================
class itString;


//============================================================================
//============================================================================
namespace mdlModelingPackageListMgr
{
	//------------------------------------------------------------------------
	//	Clear the list
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//	Add a modeling package to the list
	//
	//	This should be a base name so it covers all versions of the package.
	//	For instance, "Max" would cover all versions of 3D Studio Max.
	//------------------------------------------------------------------------
	void Add( itString& i_ModelingPackageName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool IsInList( itString& i_ModelingPackageName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool IsListEmpty();
}


