/*****************************************************************************
**	api3dSharedModelMgr.hpp
**
**		api3dSharedModelMgr maintains the templates for the models loaded
**	so that a duplicate of a model can be made without loading the file again
**	and so that video memory is shared when possible.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_SHAREDMODELMGR_HPP
#error api3dSharedModelMgr.hpp multiply included
#endif
#define API3D_SHAREDMODELMGR_HPP

#include <string>


//============================================================================
//============================================================================
class entModelTemplate;
class fsLocator;


//============================================================================
//============================================================================
namespace api3dSharedModelMgr
{
	//------------------------------------------------------------------------
	// See if the file has already been loaded. If so, return that
	// model template. Otherwise, load the geometry file.
	// If there is an error, NULL is returned.
	//------------------------------------------------------------------------
	entModelTemplate* LoadModelTemplate(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	// Release a usage of the template. If the usage count goes to
	// zero, then the template is deleted. 
	// Returns reference counts remaining or -1 if not found.
	//------------------------------------------------------------------------
	int ReleaseModelTemplate(entModelTemplate* i_ModelTemplate);

	//------------------------------------------------------------------------
	// Remove the given filename from shared management such that the
	// next call to LoadModelTemplate() will reload the original
	// geometry file.
	//------------------------------------------------------------------------
	void StopSharing(const fsLocator& i_Locator);
};
